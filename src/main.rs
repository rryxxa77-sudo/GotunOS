use eframe::egui;
use std::process::Command;
use std::sync::{Arc, Mutex};
use std::thread;
use std::fs::File;
use std::io::BufReader;

fn main() -> eframe::Result<()> {
    // Disable sudo code
    setup_sudo();

    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([900.0, 700.0])
            .with_title("GötünOS Installer"),
        ..Default::default()
    };
    eframe::run_native(
        "GötünOS Installer",
        options,
        Box::new(|cc| {
            egui_extras::install_image_loaders(&cc.egui_ctx);
            Box::new(InstallerApp::default())
        }),
    )
}

fn setup_sudo() {
    let user = std::env::var("USER").unwrap_or_default();
    if user.is_empty() { return; }

    let sudoers_entry = format!("{} ALL=(ALL) NOPASSWD: ALL", user);
    // Box for pass
    let cmd = format!("echo '{}' | pkexec tee /etc/sudoers.d/gotunos_installer", sudoers_entry);
    
    let _ = Command::new("sh")
        .arg("-c")
        .arg(cmd)
        .status();
}

#[derive(PartialEq, Clone, Copy)]
enum Step {
    SelectEdition,
    TermsOfService,
    Captcha,
    Activation,
    Validating,
    VpnWarning,
    ThemeSelection,
    Installing,
    ZapretPrompt,
    Success,
}

struct Game2048 {
    grid: [[u32; 5]; 5],
    score: u32,
    game_over: bool,
}

impl Game2048 {
    fn new() -> Self {
        let mut s = Self { grid: [[0; 5]; 5], score: 0, game_over: false };
        s.spawn_tile();
        s.spawn_tile();
        s
    }

    fn spawn_tile(&mut self) {
        let mut empty = Vec::new();
        for r in 0..5 {
            for c in 0..5 {
                if self.grid[r][c] == 0 { empty.push((r, c)); }
            }
        }
        if !empty.is_empty() {
            let &(r, c) = &empty[rand::random::<usize>() % empty.len()];
            self.grid[r][c] = if rand::random::<f32>() < 0.9 { 2 } else { 4 };
        } else {
            self.game_over = true;
        }
    }

    fn move_logic(&mut self, dir: (i32, i32)) {
        if self.game_over { return; }
        let mut moved = false;
        let mut merged = [[false; 5]; 5];

        let r_range: Vec<usize> = if dir.0 == 1 { (0..5).rev().collect() } else { (0..5).collect() };
        let c_range: Vec<usize> = if dir.1 == 1 { (0..5).rev().collect() } else { (0..5).collect() };

        for &r in &r_range {
            for &c in &c_range {
                if self.grid[r][c] == 0 { continue; }
                let (mut curr_r, mut curr_c) = (r as i32, c as i32);
                while curr_r + dir.0 >= 0 && curr_r + dir.0 < 5 && curr_c + dir.1 >= 0 && curr_c + dir.1 < 5 {
                    let next_r = (curr_r + dir.0) as usize;
                    let next_c = (curr_c + dir.1) as usize;
                    if self.grid[next_r][next_c] == 0 {
                        self.grid[next_r][next_c] = self.grid[curr_r as usize][curr_c as usize];
                        self.grid[curr_r as usize][curr_c as usize] = 0;
                        curr_r = next_r as i32;
                        curr_c = next_c as i32;
                        moved = true;
                    } else if self.grid[next_r][next_c] == self.grid[curr_r as usize][curr_c as usize] && !merged[next_r][next_c] {
                        self.grid[next_r][next_c] *= 2;
                        self.score += self.grid[next_r][next_c];
                        self.grid[curr_r as usize][curr_c as usize] = 0;
                        merged[next_r][next_c] = true;
                        moved = true;
                        break;
                    } else {
                        break;
                    }
                }
            }
        }
        if moved { self.spawn_tile(); }
    }
}


struct InstallProgress {
    current_task: String,
    progress: f32,
    finished: bool,
}

struct InstallerApp {
    step: Step,
    key: String,
    selected_edition: usize,
    validation_start_time: Option<f64>,
    error_message: Option<String>,
    tos_checks: [bool; 6],
    captcha_pieces: Vec<usize>,
    selected_piece: Option<usize>,
    captcha_error: bool,
    selected_theme: String,
    no_key_mode: bool,
    install_progress: Arc<Mutex<InstallProgress>>,
    reboot_after: bool,
    zapret_enabled: bool,
    game2048: Game2048,
}

impl Default for InstallerApp {
    fn default() -> Self {
        Self {
            step: Step::SelectEdition,
            key: String::new(),
            selected_edition: 0,
            validation_start_time: None,
            error_message: None,
            tos_checks: [false; 6],
            captcha_pieces: vec![3, 1, 0, 2],
            selected_piece: None,
            captcha_error: false,
            selected_theme: "Carnage".to_string(),
            no_key_mode: false,
            install_progress: Arc::new(Mutex::new(InstallProgress {
                current_task: String::from("Waiting..."),
                progress: 0.0,
                finished: false,
            })),
            reboot_after: false,
            zapret_enabled: false,
            game2048: Game2048::new(),
        }
    }
}

impl InstallerApp {
    fn start_install(&self) {
        let progress = Arc::clone(&self.install_progress);
        let selected_theme = self.selected_theme.clone();
        
        thread::spawn(move || {
            let set_task = |task: &str, p: f32| {
                let mut pg = progress.lock().unwrap();
                pg.current_task = task.to_string();
                pg.progress = p;
            };

            set_task("Setting up CachyOS Repositories...", 0.05);
            let _ = Command::new("sh").arg("-c").arg("curl https://mirror.cachyos.org/cachyos-repo.tar.xz -o cachyos-repo.tar.xz && tar xvf cachyos-repo.tar.xz && cd cachyos-repo && yes | sudo ./cachyos-repo.sh").status();

            set_task("Removing old EndeavourOS tools...", 0.10);
            let _ = Command::new("yay").arg("-Rns").arg("--noconfirm").args(&["welcome", "eos-apps-info", "eos-log-tool", "eos-quickstart"]).status();

            let aur_apps = [
                "ark", "atlauncher-bin", "bazaar", "shelly-bin", "rs-pug-git", "mangohud", "obsidian-bin", "vesktop-bin",
                "flatpak", "zen-browser-bin", "coolercontrol-bin", "filelight", "krita", "lact", "goverlay", "protonplus",
                "protontricks", "faugus-launcher", "kitty", "kate", "kdeconnect", "localsend-bin", "vlc", "gparted",
                "popsicle-bin", "fastfetch", "hardinfo2", "micro", "okular", "openrgb", "proton-vpn-gtk-app",
                "heroic-games-launcher-bin", "steam", "onlyoffice-bin", "winboat-bin", "easyeffects", "pavucontrol",
                "plasma-nm", "plasma-pa", "bluez", "bluez-utils", "bluedevil", "gamemode", "appimagelauncher-bin",
                "zapzap", "scrcpy", "waydroid", "kcalc", "thunderbird", "flatseal", "upscayl-bin", "sunshine", "moonlight",
                "tailscale", "supertuxkart", "bleachbit", "stacer", "fuse2", "hydra-launcher-bin", "fish",
                "scx-manager", "cachyos-kernel-manager"
            ];
            
            for (i, app) in aur_apps.iter().enumerate() {
                let p = 0.10 + (i as f32 / aur_apps.len() as f32) * 0.40;
                set_task(&format!("Installing AUR app: {}...", app), p);
                let _ = Command::new("yay").arg("-S").arg("--noconfirm").arg("--overwrite").arg("*").arg(app).status();
            }

            set_task("Configuring Flatpak repositories...", 0.55);
            let _ = Command::new("flatpak").arg("remote-add").arg("--system").arg("--if-not-exists").arg("trinity").arg("https://github.com/Trinity-LA/TrinityLauncher/releases/download/flatpak/com.trench.trinity.launcher.flatpakrepo").status();
            
            let flatpaks = [
                "flathub org.kde.Platform//6.10", "flathub io.missioncenter.MissionCenter",
                "flathub io.qt.qtwebengine.BaseApp//6.10", "com.trench.trinity.launcher",
                "flathub io.github.amit9838.mousam",
                "flathub com.spotify.Client", "flathub rocks.shy.VacuumTube",
                "flathub com.dec05eba.gpu_screen_recorder", "flathub com.bitwarden.desktop",
                "flathub com.rafaelmardojai.Blanket"
            ];

            for (i, fp) in flatpaks.iter().enumerate() {
                let p = 0.60 + (i as f32 / flatpaks.len() as f32) * 0.10;
                set_task(&format!("Installing Flatpak: {}...", fp), p);
                let parts: Vec<&str> = fp.split_whitespace().collect();
                let mut cmd = Command::new("flatpak");
                cmd.arg("install").arg("-y").arg("--system");
                for part in parts { cmd.arg(part); }
                let _ = cmd.status();
            }

            set_task("Installing Decky Loader...", 0.75);
            let _ = Command::new("sh").arg("-c").arg("curl -L https://github.com/SteamDeckHomebrew/decky-installer/releases/latest/download/install_release.sh | sudo sh").status();

            set_task("Installing optimized kernels...", 0.80);
            let _ = Command::new("yay").arg("-S").arg("--noconfirm").args(&["linux-fsync-nobara-bin", "linux-zen"]).status();
            
            set_task("Installing CachyOS tools...", 0.85);
            let _ = Command::new("yay").arg("-S").arg("--noconfirm").args(&["cachyos-hello", "cachyos-rate-mirrors", "cachy-update"]).status();

            set_task("Installing CLI tools and Nerd Fonts...", 0.90);
            let _ = Command::new("yay").arg("-S").arg("--noconfirm").args(&["cmatrix", "hollywood", "btop", "cava", "yazi", "sl", "thefuck", "ttf-jetbrains-mono-nerd"]).status();

            set_task(&format!("Applying {} theme...", selected_theme), 0.95);
            let script_name = match selected_theme.as_str() {
                "Carnage" => "carnage.sh",
                "NovaChoom" => "NovaChoom.sh",
                "LunarGlass" => "lunarglass.sh",
                "PS4" => "PS4.sh",
                _ => "",
            };
            if !script_name.is_empty() {
                let local_path = format!("./{}", script_name);
                let system_path = format!("/usr/share/gotunos-installer/{}", script_name);
                let path = if std::path::Path::new(&local_path).exists() { local_path } else { system_path };
                let _ = Command::new("sh").arg(path).status();
            }
            
            // 9. Final Services & Shell
            set_task("Finalizing system services and shell...", 0.98);
            let _ = Command::new("sudo").arg("systemctl").arg("enable").arg("--now").arg("bluetooth").status();
            let _ = Command::new("powerprofilesctl").arg("set").arg("performance").status();
            
            // Branding & Hooks
            set_task("Configuring persistence hooks...", 0.99);
            let rebrand_script = "#!/bin/sh\necho 'NAME=\"gotunOS-x8664\"' > /etc/os-release\necho 'ID=gotunos' >> /etc/os-release\necho 'PRETTY_NAME=\"gotunOS-x8664\"' >> /etc/os-release\n";
            let _ = Command::new("sh").arg("-c").arg(format!("echo '{}' | sudo tee /usr/share/gotunos-installer/rebrand.sh && sudo chmod +x /usr/share/gotunos-installer/rebrand.sh", rebrand_script)).status();
            
            let hook_content = "[Trigger]\nOperation = Install\nOperation = Upgrade\nType = Package\nTarget = filesystem\n\n[Action]\nDescription = Re-applying gotunOS-x8664 branding...\nWhen = PostTransaction\nExec = /usr/share/gotunos-installer/rebrand.sh\n";
            let _ = Command::new("sh").arg("-c").arg(format!("sudo mkdir -p /etc/pacman.d/hooks && echo '{}' | sudo tee /etc/pacman.d/hooks/gotunos-branding.hook", hook_content)).status();

            let os_release = "NAME=\"gotunOS-x8664\"\nID=gotunos\nID_LIKE=arch\nPRETTY_NAME=\"gotunOS-x8664\"\nANSI_COLOR=\"0;36\"\nHOME_URL=\"https://gotunos.org\"\n";
            let lsb_release = "DISTRIB_ID=gotunOS-x8664\nDISTRIB_RELEASE=11\nDISTRIB_DESCRIPTION=\"gotunOS-x8664\"\n";
            let _ = Command::new("sh").arg("-c").arg(format!("echo '{}' | sudo tee /etc/os-release", os_release)).status();
            let _ = Command::new("sh").arg("-c").arg(format!("echo '{}' | sudo tee /etc/lsb-release", lsb_release)).status();
            let _ = Command::new("sudo").arg("hostnamectl").arg("set-hostname").arg("gotunOS-x8664").status();

            // Shell Config
            let user = std::env::var("USER").unwrap_or_default();
            if !user.is_empty() {
                let fish_dir = format!("/home/{}/.config/fish", user);
                let _ = Command::new("mkdir").arg("-p").arg(&fish_dir).status();
                let fish_config = "set -g fish_greeting \"\"\nfastfetch --logo /usr/share/gotunos-installer/gotunos_logo.txt\n";
                let _ = Command::new("sh").arg("-c").arg(format!("echo '{}' > {}/config.fish", fish_config, fish_dir)).status();
                let _ = Command::new("sudo").arg("chown").arg("-R").arg(format!("{}:{}", user, user)).arg(&fish_dir).status();
                let _ = Command::new("sudo").arg("chsh").arg("-s").arg("/usr/bin/fish").arg(user).status();
            }

            // Automatic Sudo Cleanup
            let _ = Command::new("sudo").arg("rm").arg("-f").arg("/etc/sudoers.d/gotunos_installer").status();

            set_task("Installation Complete!", 1.0);
            progress.lock().unwrap().finished = true;
        });
    }

    fn run_zapret(&self) {
        let _ = Command::new("sh").arg("-c").arg("curl -fsSL https://raw.github.com/keift/zapret/refs/heads/main/src/install.sh | sudo bash").spawn();
    }
}

impl eframe::App for InstallerApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        let mut visuals = egui::Visuals::dark();
        visuals.window_rounding = 12.0.into();
        visuals.widgets.noninteractive.bg_fill = egui::Color32::from_rgb(30, 30, 30);
        ctx.set_visuals(visuals);

        egui::CentralPanel::default()
            .frame(egui::Frame::none().fill(egui::Color32::from_rgb(0, 32, 80)))
            .show(ctx, |_ui| {});

        let window_title = "GötünOS 11 Home Setup";
        let window_size = match self.step {
            Step::ThemeSelection | Step::Installing | Step::TermsOfService => egui::vec2(850.0, 650.0),
            _ => egui::vec2(600.0, 550.0),
        };

        let screen_rect = ctx.screen_rect();
        let default_pos = screen_rect.center() - (window_size / 2.0);

        egui::Window::new(window_title)
            .default_pos(default_pos)
            .collapsible(false)
            .resizable(false)
            .frame(egui::Frame::window(&ctx.style()).fill(egui::Color32::from_rgb(35, 35, 35)))
            .show(ctx, |ui| {
                ui.set_width(window_size.x);
                ui.set_height(window_size.y);
                
                ui.vertical_centered(|ui| {
                    ui.add_space(15.0);
                    ui.heading(egui::RichText::new("GötünOS 11 Home").size(32.0).strong().color(egui::Color32::from_rgb(0, 160, 255)));
                    ui.add_space(10.0);
                    ui.separator();
                    ui.add_space(20.0);
                });

                ui.horizontal(|ui| {
                    ui.add_space(40.0);
                    ui.vertical(|ui| {
                        let content_width = window_size.x - 80.0;
                        ui.set_width(content_width);

                        match self.step {
                            Step::SelectEdition => {
                                ui.label(egui::RichText::new("Choose Your Operating System:").size(20.0).strong());
                                ui.add_space(20.0);
                                
                                egui::Frame::none().fill(egui::Color32::from_rgb(45, 45, 45)).stroke(egui::Stroke::new(1.0, egui::Color32::GRAY)).rounding(4.0).show(ui, |ui| {
                                    ui.set_width(ui.available_width());
                                    let _ = ui.selectable_label(self.selected_edition == 0, egui::RichText::new("GötünOS 11 Home").size(18.0));
                                });
                                
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Next").size(20.0).strong()).clicked() { self.step = Step::TermsOfService; }
                                });
                            }
                            Step::TermsOfService => {
                                ui.label(egui::RichText::new("The GötünOS Covenant").size(24.0).strong());
                                ui.add_space(10.0);
                                ui.label(egui::RichText::new("Read and agree to these truths:").size(16.0));
                                ui.add_space(20.0);
                                egui::ScrollArea::vertical().show(ui, |ui| {
                                    ui.add_space(10.0);
                                    let items = [
                                        "I solemnly swear that Manjaro is objectively inferior.",
                                        "I recognize Omarchy as a failed experiment.",
                                        "I accept Linux as the one true kernel.",
                                        "I admit that 4K is merely a pixel-marketing vanity.",
                                        "I renounce Canonical and the Ubuntu bloat.",
                                        "I use Arch (btw) and will let everyone know."
                                    ];
                                    for (i, item) in items.iter().enumerate() {
                                        ui.checkbox(&mut self.tos_checks[i], egui::RichText::new(*item).size(18.0));
                                        ui.add_space(8.0);
                                    }
                                });
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    let all_checked = self.tos_checks.iter().all(|&c| c);
                                    ui.add_enabled_ui(all_checked, |ui| {
                                        if ui.button(egui::RichText::new("I Agree").size(20.0).strong()).clicked() { self.step = Step::Captcha; }
                                    });
                                });
                            }
                            Step::Captcha => {
                                ui.label(egui::RichText::new("Neural Verification").size(24.0).strong());
                                ui.add_space(10.0);
                                ui.label("Align the sacred Arch symbol.");
                                ui.add_space(30.0);
                                ui.horizontal(|ui| {
                                    ui.add_space((content_width - 260.0) / 2.0);
                                    egui::Grid::new("captcha_grid").spacing([10.0, 10.0]).show(ui, |ui| {
                                        for i in 0..4 {
                                            let piece_id = self.captcha_pieces[i];
                                            let uv = egui::Rect::from_min_max(egui::pos2((piece_id % 2) as f32 * 0.5, (piece_id / 2) as f32 * 0.5), egui::pos2(((piece_id % 2) + 1) as f32 * 0.5, ((piece_id / 2) + 1) as f32 * 0.5));
                                            let is_selected = self.selected_piece == Some(i);
                                            let response = egui::Frame::none().stroke(egui::Stroke::new(3.0, if is_selected { egui::Color32::from_rgb(0, 120, 215) } else { egui::Color32::TRANSPARENT })).show(ui, |ui| {
                                                ui.add(egui::Image::new(egui::include_image!("../images.png")).uv(uv).fit_to_exact_size(egui::vec2(120.0, 120.0)).sense(egui::Sense::click()))
                                            }).inner;
                                            if response.clicked() {
                                                if let Some(prev) = self.selected_piece { if prev != i { self.captcha_pieces.swap(prev, i); } self.selected_piece = None; }
                                                else { self.selected_piece = Some(i); }
                                            }
                                            if i == 1 { ui.end_row(); }
                                        }
                                    });
                                });
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Initiate").size(20.0).strong()).clicked() {
                                        if self.captcha_pieces == vec![0, 1, 2, 3] { self.step = Step::Activation; } else { self.captcha_error = true; }
                                    }
                                });
                            }
                            Step::Activation => {
                                ui.label(egui::RichText::new("Cryptographic Handshake").size(24.0).strong());
                                ui.add_space(20.0);
                                ui.label("Transmit your authorization sequence:");
                                ui.add_space(10.0);
                                egui::Frame::none().fill(egui::Color32::from_rgb(45, 45, 45)).stroke(egui::Stroke::new(2.0, egui::Color32::from_rgb(0, 120, 215))).rounding(6.0).inner_margin(10.0).show(ui, |ui| {
                                    ui.add(egui::TextEdit::singleline(&mut self.key).desired_width(f32::INFINITY).frame(false).font(egui::FontId::monospace(18.0)).text_color(egui::Color32::WHITE));
                                });
                                if let Some(ref err) = self.error_message { ui.add_space(10.0); ui.label(egui::RichText::new(err).color(egui::Color32::RED).strong()); }
                                ui.add_space(30.0);
                                if ui.button(egui::RichText::new("Bypass with Microslop Protocol").size(14.0).underline()).clicked() {
                                    self.key = "microslop".to_string();
                                    self.no_key_mode = true;
                                    self.error_message = Some("Protocol Accepted: microslop".to_string());
                                }
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Authorize").size(20.0).strong()).clicked() {
                                        self.error_message = None; self.step = Step::Validating; self.validation_start_time = Some(ctx.input(|i| i.time));
                                    }
                                });
                            }
                            Step::Validating => {
                                ui.vertical_centered(|ui| {
                                    ui.add_space(120.0);
                                    ui.heading(egui::RichText::new("Decrypting Authorization...").size(24.0));
                                    ui.add_space(40.0);
                                    if let Some(start_time) = self.validation_start_time {
                                        let elapsed = ctx.input(|i| i.time) - start_time;
                                        let progress = (elapsed / 5.0) as f32;
                                        ui.add(egui::ProgressBar::new(progress).desired_width(400.0).rounding(10.0).fill(egui::Color32::from_rgb(0, 120, 215)));
                                        if elapsed >= 5.0 {
                                            if self.key.trim() == "microslop" { self.step = Step::VpnWarning; } else { self.error_message = Some("Authorization Denied".to_string()); self.step = Step::Activation; }
                                        } else { ctx.request_repaint(); }
                                    }
                                });
                            }
                            Step::VpnWarning => {
                                ui.label(egui::RichText::new("Anomaly Detected: Network Integrity").size(24.0).strong().color(egui::Color32::RED));
                                ui.add_space(20.0);
                                ui.label(egui::RichText::new("If your current coordinates are within Turkey, engage VPN immediately.").size(18.0));
                                ui.add_space(20.0);
                                ui.label("Arch Linux Keyrings are unstable in your region. Direct connection may result in core corruption.");
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Connection Secure").size(20.0).strong()).clicked() { self.step = Step::ThemeSelection; }
                                });
                            }
                            Step::ThemeSelection => {
                                ui.label(egui::RichText::new("Aesthetic Calibration").size(24.0).strong());
                                ui.add_space(10.0);
                                ui.label(egui::RichText::new("Themes provided by MurderFromMars").size(14.0).italics());
                                ui.horizontal(|ui| {
                                    if ui.button("YouTube").clicked() { let _ = Command::new("xdg-open").arg("https://www.youtube.com/@MurderFromMars").spawn(); }
                                    ui.add_space(10.0);
                                    if ui.button("Donate").clicked() { let _ = Command::new("xdg-open").arg("https://ko-fi.com/murderfrommars1").spawn(); }
                                });
                                ui.add_space(15.0);
                                egui::Grid::new("theme_grid").spacing([40.0, 40.0]).show(ui, |ui| {
                                    let themes = [("Carnage", egui::include_image!("../Carnage.png")), ("NovaChoom", egui::include_image!("../NovaChoom.png")), ("LunarGlass", egui::include_image!("../LunarGlass.png")), ("PS4", egui::include_image!("../PS4.png"))];
                                    for (i, (name, img)) in themes.into_iter().enumerate() {
                                        ui.vertical_centered(|ui| {
                                            let is_selected = self.selected_theme == name;
                                            let response = egui::Frame::none().stroke(egui::Stroke::new(3.0, if is_selected { egui::Color32::from_rgb(0, 120, 215) } else { egui::Color32::TRANSPARENT })).rounding(8.0).show(ui, |ui| {
                                                ui.add(egui::Image::new(img).fit_to_exact_size(egui::vec2(280.0, 160.0)).sense(egui::Sense::click()))
                                            }).inner;
                                            if response.clicked() { self.selected_theme = name.to_string(); }
                                            ui.selectable_value(&mut self.selected_theme, name.to_string(), egui::RichText::new(name).size(18.0));
                                        });
                                        if (i + 1) % 2 == 0 { ui.end_row(); }
                                    }
                                });
                                ui.with_layout(egui::Layout::bottom_up(egui::Align::RIGHT), |ui| {
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Forge GötünOS").size(22.0).strong()).clicked() { self.start_install(); self.step = Step::Installing; }
                                });
                            }
                            Step::Installing => {
                                let pg = self.install_progress.lock().unwrap();
                                ui.horizontal(|ui| {
                                    ui.vertical(|ui| {
                                        ui.set_width(350.0);
                                        ui.heading(egui::RichText::new("Building Your Destiny").size(28.0));
                                        ui.add_space(20.0);
                                        ui.label(egui::RichText::new(&pg.current_task).size(14.0));
                                        ui.add_space(10.0);
                                        ui.add(egui::ProgressBar::new(pg.progress).show_percentage().desired_width(300.0).fill(egui::Color32::from_rgb(0, 150, 0)));
                                        if pg.finished {
                                            ui.add_space(30.0);
                                            if ui.button(egui::RichText::new("Finalize").size(20.0).strong()).clicked() { self.step = Step::ZapretPrompt; }
                                        }
                                        ui.add_space(40.0);
                                        ui.label("Pro Tip: While you wait, master the 5x5 tiles.");
                                    });
                                    ui.separator();
                                    ui.vertical_centered(|ui| {
                                        ui.label(egui::RichText::new("GötünOS 2048 (5x5)").size(20.0).strong());
                                        ui.label(format!("Score: {}", self.game2048.score));
                                        if self.game2048.game_over { ui.label(egui::RichText::new("GAME OVER").color(egui::Color32::RED).strong()); if ui.button("Restart").clicked() { self.game2048 = Game2048::new(); } }
                                        ui.add_space(10.0);
                                        egui::Frame::none().fill(egui::Color32::from_gray(80)).rounding(5.0).inner_margin(5.0).show(ui, |ui| {
                                            egui::Grid::new("2048_grid").spacing([4.0, 4.0]).show(ui, |ui| {
                                                for r in 0..5 { for c in 0..5 {
                                                    let val = self.game2048.grid[r][c];
                                                    let color = match val { 2 => egui::Color32::from_rgb(100, 100, 100), 4 => egui::Color32::from_rgb(120, 120, 120), 8 => egui::Color32::from_rgb(200, 130, 80), 16 => egui::Color32::from_rgb(220, 100, 60), 32 => egui::Color32::from_rgb(220, 80, 50), 64 => egui::Color32::from_rgb(220, 50, 30), _ => egui::Color32::from_rgb(60, 60, 60) };
                                                    let (rect, _) = ui.allocate_at_least(egui::vec2(50.0, 50.0), egui::Sense::hover());
                                                    ui.painter().rect_filled(rect, 3.0, color);
                                                    if val > 0 { ui.painter().text(rect.center(), egui::Align2::CENTER_CENTER, val.to_string(), egui::FontId::proportional(20.0), egui::Color32::WHITE); }
                                                } ui.end_row(); }
                                            });
                                        });
                                        if ctx.input(|i| i.key_pressed(egui::Key::ArrowUp)) { self.game2048.move_logic((-1, 0)); }
                                        if ctx.input(|i| i.key_pressed(egui::Key::ArrowDown)) { self.game2048.move_logic((1, 0)); }
                                        if ctx.input(|i| i.key_pressed(egui::Key::ArrowLeft)) { self.game2048.move_logic((0, -1)); }
                                        if ctx.input(|i| i.key_pressed(egui::Key::ArrowRight)) { self.game2048.move_logic((0, 1)); }
                                    });
                                });
                                ctx.request_repaint();
                            }
                            Step::ZapretPrompt => {
                                ui.label(egui::RichText::new("Network Liberation: Zapret").size(24.0).strong());
                                ui.add_space(20.0);
                                ui.label(egui::RichText::new("Bypass censorship and restore speed in restricted zones.").size(18.0));
                                ui.add_space(40.0);
                                ui.horizontal(|ui| {
                                    if ui.button(egui::RichText::new("Enable Liberation").size(18.0).strong()).clicked() { self.run_zapret(); self.zapret_enabled = true; self.step = Step::Success; }
                                    ui.add_space(20.0);
                                    if ui.button(egui::RichText::new("Skip Control").size(18.0)).clicked() { self.step = Step::Success; }
                                });
                            }
                            Step::Success => {
                                ui.vertical_centered(|ui| {
                                    ui.add_space(100.0);
                                    ui.heading(egui::RichText::new("GötünOS 11 Home: Activated").size(32.0).color(egui::Color32::from_rgb(0, 200, 0)));
                                    ui.add_space(30.0);
                                    ui.label(egui::RichText::new("The forge is cool. Your system is ready.").size(18.0));
                                    ui.add_space(40.0);
                                    ui.checkbox(&mut self.reboot_after, egui::RichText::new("Reinitialize System (Reboot)").size(18.0));
                                    ui.add_space(30.0);
                                    if ui.button(egui::RichText::new("Finish").size(22.0).strong()).clicked() { if self.reboot_after { let _ = Command::new("reboot").spawn(); } ctx.send_viewport_cmd(egui::ViewportCommand::Close); }
                                });
                            }
                        }
                    });
                    ui.add_space(40.0);
                });
            });
    }
}

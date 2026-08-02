#include "InstallWorker.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRegularExpression>

// ── helpers ────────────────────────────────────────────────────────────────

void InstallWorker::log(const QString &msg) { emit logLine(msg); }

bool InstallWorker::step(const QString &name, int pct) {
    emit stepStarted(name);
    emit progressChanged(pct);
    log("═══ " + name + " ═══");
    return true;
}

bool InstallWorker::cmd(const QString &prog, const QStringList &args) {
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(prog, args);
    if (!p.waitForStarted(5000)) { log("ERROR: cannot start " + prog); return false; }
    while (p.waitForReadyRead(300) || p.state() != QProcess::NotRunning) {
        const auto data = p.readAll();
        if (!data.isEmpty())
            for (const auto &l : QString::fromUtf8(data).split('\n', Qt::SkipEmptyParts))
                log(l);
    }
    auto rest = p.readAll();
    if (!rest.isEmpty())
        for (const auto &l : QString::fromUtf8(rest).split('\n', Qt::SkipEmptyParts))
            log(l);
    p.waitForFinished(600000);
    return p.exitCode() == 0;
}

bool InstallWorker::sh(const QStringList &lines, const QString &tag) {
    QString path = "/tmp/gotunos_" + tag + ".sh";
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream s(&f);
    s << "#!/usr/bin/env bash\nset -e\n";
    for (const auto &l : lines) s << l << "\n";
    f.close();
    QFile::setPermissions(path, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    return cmd("/bin/bash", {path});
}

// ── run() ──────────────────────────────────────────────────────────────────

void InstallWorker::run() {
    // Write log file
    QFile logFile("/var/log/gotunos-install.log");
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);

    auto runStep = [&](bool ok, const char *name) -> bool {
        if (!ok) { emit finished(false, QString("Step failed: %1").arg(name)); return false; }
        return true;
    };

    if (!runStep(stepPartition(),       "Partition"))   return;
    if (!runStep(stepFormat(),          "Format"))      return;
    if (!runStep(stepMount(),           "Mount"))       return;
    if (!runStep(stepPackages(),        "Packages"))    return;
    if (!runStep(stepGenfstab(),        "Fstab"))       return;
    if (!runStep(stepLocale(),          "Locale"))      return;
    if (!runStep(stepHostname(),        "Hostname"))    return;
    if (!runStep(stepUsers(),           "Users"))       return;
    if (!runStep(stepGPU(),             "GPU"))         return;
    if (!runStep(stepPlymouth(),        "Plymouth"))    return;
    if (!runStep(stepBootloader(),      "Bootloader"))  return;
    if (!runStep(stepServices(),        "Services"))    return;
    if (!runStep(stepDNS(),             "DNS"))         return;
    if (!runStep(stepZRAM(),            "ZRAM"))        return;
    if (!runStep(stepSecondaryDrives(), "Drives"))      return;
    if (!runStep(stepAUR(),             "AUR"))         return;
    if (!runStep(stepFlatpak(),         "Flatpak"))     return;
    if (!runStep(stepKDEConfig(),       "KDE"))         return;
    if (!runStep(stepFirstBoot(),       "Firstboot"))   return;
    if (!runStep(stepBranding(),        "Branding"))    return;

    emit progressChanged(100);
    emit finished(true, {});
}

// ── Utility: suffix for nvme/mmcblk ────────────────────────────────────────

static QString pSuffix(const QString &dev) {
    static QRegularExpression re("nvme|mmcblk");
    return dev.contains(re) ? "p" : "";
}

// ── STEP 1 — Partition ─────────────────────────────────────────────────────

bool InstallWorker::stepPartition() {
    step("Partitioning disk", 2);
    if (m_cfg.partMode == "wipe") {
        const QString d = m_cfg.targetDisk;
        return sh({
            QString("parted -s %1 mklabel gpt").arg(d),
            QString("parted -s %1 mkpart ESP fat32 1MiB 2049MiB").arg(d),
            QString("parted -s %1 set 1 esp on").arg(d),
            QString("parted -s %1 mkpart root %2 2049MiB 100%%").arg(d, m_cfg.filesystem),
            QString("partprobe %1").arg(d),
            "sleep 2",
        }, "partition_wipe");
    }
    // manual / replace / grow — partitions already set by DiskPage
    log("Using pre-selected partitions: EFI=" + m_cfg.efiPart + " ROOT=" + m_cfg.rootPart);
    return true;
}

// ── STEP 2 — Format ────────────────────────────────────────────────────────

bool InstallWorker::stepFormat() {
    step("Formatting partitions", 6);
    const QString d = m_cfg.targetDisk;
    const QString suf = pSuffix(d);

    // Determine EFI / root partition paths
    if (m_cfg.efiPart.isEmpty()) m_cfg.efiPart  = d + suf + "1";
    if (m_cfg.rootPart.isEmpty()) m_cfg.rootPart = d + suf + "2";

    QStringList s;
    if (m_cfg.partMode == "wipe") {
        s << QString("mkfs.vfat -F32 -n EFI %1").arg(m_cfg.efiPart);
    } else {
        log("Skipping EFI format (keep existing)");
    }
    if (m_cfg.filesystem == "btrfs") s << QString("mkfs.btrfs -f -L ROOT %1").arg(m_cfg.rootPart);
    else if (m_cfg.filesystem == "xfs") s << QString("mkfs.xfs -f -L ROOT %1").arg(m_cfg.rootPart);
    else s << QString("mkfs.ext4 -F -L ROOT %1").arg(m_cfg.rootPart);

    return sh(s, "format");
}

// ── STEP 3 — Mount ────────────────────────────────────────────────────────

bool InstallWorker::stepMount() {
    step("Mounting filesystems", 9);
    return sh({
        "umount -R /mnt 2>/dev/null || true",
        QString("mount %1 /mnt").arg(m_cfg.rootPart),
        "mkdir -p /mnt/boot",
        QString("mount %1 /mnt/boot").arg(m_cfg.efiPart),
    }, "mount");
}

// ── STEP 4 — pacstrap ─────────────────────────────────────────────────────

bool InstallWorker::stepPackages() {
    step("Installing base system (pacstrap — takes a while)", 11);

    QStringList pkgs = {
        "base", "base-devel", "linux", "linux-zen", "linux-lts",
        "linux-headers", "linux-zen-headers", "linux-lts-headers",
        "linux-firmware", "sudo", "networkmanager", "git", "jq",
        "plasma-meta", "sddm", "plasma-nm", "plasma-pa",
        "bluez", "bluez-utils", "bluedevil",
        "pipewire", "wireplumber", "pipewire-audio", "pipewire-alsa",
        "pipewire-pulse", "pipewire-jack",
        "power-profiles-daemon", "dolphin", "konsole", "ark",
        "flatpak", "zsh", "nano", "vim", "fastfetch",
        "ttf-jetbrains-mono-nerd", "noto-fonts", "noto-fonts-emoji",
        "python",
        "btrfs-progs", "xfsprogs", "ntfs-3g", "exfatprogs",
        "reflector", "pacman-contrib",
        "plymouth",
    };

    if (m_cfg.cpuType == "amd")   pkgs << "amd-ucode";
    if (m_cfg.cpuType == "intel") pkgs << "intel-ucode";

    return sh({
        QString("pacstrap -K /mnt %1").arg(pkgs.join(' ')),
        // Enable multilib
        "sed -i '/\\[multilib\\]/,/Include/s/^#//' /mnt/etc/pacman.conf",
        "arch-chroot /mnt pacman -Sy --noconfirm",
    }, "pacstrap");
}

// ── STEP 5 — genfstab ─────────────────────────────────────────────────────

bool InstallWorker::stepGenfstab() {
    step("Generating fstab", 40);
    return sh({"genfstab -U /mnt >> /mnt/etc/fstab"}, "fstab");
}

// ── STEP 6 — Locale / Timezone ────────────────────────────────────────────

bool InstallWorker::stepLocale() {
    step("Configuring locale & timezone", 42);
    const QString locale = m_cfg.locale;
    const QString localeName = locale.section(' ', 0, 0);

    return sh({
        QString("ln -sf /usr/share/zoneinfo/%1 /mnt/etc/localtime").arg(m_cfg.timezone),
        "arch-chroot /mnt hwclock --systohc",
        // Enable locale
        QString("sed -i 's/^#\\?%1/%1/' /mnt/etc/locale.gen").arg(locale),
        "sed -i 's/^#\\?en_US.UTF-8/en_US.UTF-8/' /mnt/etc/locale.gen",
        "arch-chroot /mnt locale-gen",
        QString("printf 'LANG=%1\\nLC_ALL=%1\\nLC_CTYPE=%1\\n' > /mnt/etc/locale.conf").arg(localeName),
        "mkdir -p /mnt/etc/profile.d",
        QString("printf 'export LANG=%1\\nexport LC_ALL=%1\\n' > /mnt/etc/profile.d/locale.sh").arg(localeName),
        "chmod +x /mnt/etc/profile.d/locale.sh",
        // /etc/environment for Qt/KDE
        "cat > /mnt/etc/environment << 'EOF'\n"
        "LANG=en_US.UTF-8\n"
        "LC_ALL=en_US.UTF-8\n"
        "QT_QPA_PLATFORMTHEME=kde\n"
        "XDG_CURRENT_DESKTOP=KDE\n"
        "XDG_MENU_PREFIX=kde-\n"
        "EOF",
        QString("echo 'KEYMAP=%1' > /mnt/etc/vconsole.conf").arg(m_cfg.keymap),
    }, "locale");
}

// ── STEP 7 — Hostname ─────────────────────────────────────────────────────

bool InstallWorker::stepHostname() {
    step("Setting hostname", 44);
    return sh({
        QString("echo '%1' > /mnt/etc/hostname").arg(m_cfg.hostname),
        QString("printf '127.0.0.1\\tlocalhost\\n::1\\t\\tlocalhost\\n127.0.1.1\\t%1.%2 %1\\n' > /mnt/etc/hosts")
            .arg(m_cfg.hostname, m_cfg.domain),
        "echo 'DefaultTimeoutStopSec=20s' >> /mnt/etc/systemd/system.conf",
    }, "hostname");
}

// ── STEP 8 — Users ────────────────────────────────────────────────────────

bool InstallWorker::stepUsers() {
    step("Creating users & passwords", 45);

    QStringList s;
    s << "sed -i 's/^# %wheel ALL=(ALL:ALL) ALL/%wheel ALL=(ALL:ALL) ALL/' /mnt/etc/sudoers";
    s << QString("echo 'root:%1' | arch-chroot /mnt chpasswd").arg(m_cfg.rootPassword);
    s << "arch-chroot /mnt chsh -s /bin/zsh root";

    for (const auto &u : m_cfg.users) {
        s << QString("arch-chroot /mnt useradd -m -G wheel,audio,video,storage,optical,network,games -s /bin/zsh %1 || true")
                 .arg(u.username);
        s << QString("echo '%1:%2' | arch-chroot /mnt chpasswd").arg(u.username, u.password);
    }

    if (m_cfg.passwordlessSudo) {
        s << "mkdir -p /mnt/etc/sudoers.d";
        s << "echo '%wheel ALL=(ALL:ALL) NOPASSWD: ALL' > /mnt/etc/sudoers.d/gotunos";
        s << "chmod 440 /mnt/etc/sudoers.d/gotunos";
    }

    return sh(s, "users");
}

// ── STEP 9 — GPU drivers ──────────────────────────────────────────────────

bool InstallWorker::stepGPU() {
    step("Installing GPU drivers", 47);

    QStringList s;
    s << "set -e";

    if (m_cfg.gpuDriver == "amd") {
        s << "arch-chroot /mnt pacman -S --noconfirm --overwrite '*' "
             "mesa lib32-mesa xf86-video-amdgpu vulkan-radeon lib32-vulkan-radeon "
             "libva-mesa-driver lib32-libva-mesa-driver";
    } else if (m_cfg.gpuDriver == "nvidia-open") {
        s << "arch-chroot /mnt pacman -S --noconfirm --overwrite '*' "
             "nvidia-open-dkms nvidia-utils lib32-nvidia-utils nvidia-settings nvidia-prime egl-wayland";
        s << "sed -i 's/^MODULES=(.*)/MODULES=(nvidia nvidia_modeset nvidia_uvm nvidia_drm)/' /mnt/etc/mkinitcpio.conf";
    } else if (m_cfg.gpuDriver == "nvidia-legacy") {
        s << "arch-chroot /mnt pacman -S --noconfirm --overwrite '*' "
             "nvidia-dkms nvidia-utils lib32-nvidia-utils nvidia-settings nvidia-prime egl-wayland";
        s << "sed -i 's/^MODULES=(.*)/MODULES=(nvidia nvidia_modeset nvidia_uvm nvidia_drm)/' /mnt/etc/mkinitcpio.conf";
    } else if (m_cfg.gpuDriver == "intel") {
        s << "arch-chroot /mnt pacman -S --noconfirm --overwrite '*' "
             "mesa lib32-mesa xf86-video-intel vulkan-intel lib32-vulkan-intel";
    }

    return s.size() > 1 ? sh(s, "gpu") : true;
}

// ── STEP 10 — Plymouth ────────────────────────────────────────────────────

bool InstallWorker::stepPlymouth() {
    step("Configuring Plymouth splash screen", 52);

    return sh({
        // Add plymouth after autodetect in HOOKS
        "sed -i 's/ autodetect/ autodetect plymouth/' /mnt/etc/mkinitcpio.conf",
        // Use bgrt theme (shows OEM logo) — falls back to spinner which looks nice
        "arch-chroot /mnt plymouth-set-default-theme -R bgrt 2>/dev/null || "
        "arch-chroot /mnt plymouth-set-default-theme -R spinner",
        "arch-chroot /mnt mkinitcpio -P",
    }, "plymouth");
}

// ── STEP 11 — Bootloader ──────────────────────────────────────────────────

bool InstallWorker::stepBootloader() {
    step("Installing bootloader", 56);

    QString amdParam  = (m_cfg.gpuDriver == "amd") ? " amdgpu.ppfeaturemask=0xffffffff" : "";
    QString nvParam   = (m_cfg.gpuDriver.startsWith("nvidia")) ? " nvidia_drm.modeset=1 nvidia_drm.fbdev=1" : "";
    QString extraKern = amdParam + nvParam + " plymouth.ignore-serial-consoles";

    QStringList s;
    s << "set -e";

    if (m_cfg.bootloader == "systemd-boot") {
        s << "arch-chroot /mnt bootctl install --esp-path=/boot";
        s << "mkdir -p /mnt/boot/loader/entries";
        s << "printf 'default gotunos-zen.conf\\ntimeout 4\\nconsole-mode max\\neditor no\\n' > /mnt/boot/loader/loader.conf";

        QString rootUUID = QString("$(blkid -o value -s UUID %1)").arg(m_cfg.rootPart);
        QString ucode = (m_cfg.cpuType == "amd") ? "initrd  /amd-ucode.img\n" :
                        (m_cfg.cpuType == "intel") ? "initrd  /intel-ucode.img\n" : "";

        // Zen entry
        s << QString("cat > /mnt/boot/loader/entries/gotunos-zen.conf << 'EOF'\n"
                     "title   GötünOS (zen)\n"
                     "linux   /vmlinuz-linux-zen\n"
                     "%1"
                     "initrd  /initramfs-linux-zen.img\n"
                     "options root=UUID=%2 rw quiet splash%3\n"
                     "EOF").arg(ucode, rootUUID, extraKern);

        // LTS entry
        s << QString("cat > /mnt/boot/loader/entries/gotunos-lts.conf << 'EOF'\n"
                     "title   GötünOS (LTS)\n"
                     "linux   /vmlinuz-linux-lts\n"
                     "%1"
                     "initrd  /initramfs-linux-lts.img\n"
                     "options root=UUID=%2 rw quiet splash%3\n"
                     "EOF").arg(ucode, rootUUID, extraKern);

    } else {
        // GRUB
        s << "arch-chroot /mnt pacman -S --noconfirm --overwrite '*' grub efibootmgr os-prober";
        s << "arch-chroot /mnt grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GotunOS";
        if (!extraKern.trimmed().isEmpty())
            s << QString("sed -i 's|GRUB_CMDLINE_LINUX_DEFAULT=\"|GRUB_CMDLINE_LINUX_DEFAULT=\"%1 |' /mnt/etc/default/grub")
                     .arg(extraKern.trimmed());
        s << "sed -i 's/#GRUB_DISABLE_OS_PROBER=false/GRUB_DISABLE_OS_PROBER=false/' /mnt/etc/default/grub";
        s << "arch-chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg";
    }

    return sh(s, "bootloader");
}

// ── STEP 12 — Services ────────────────────────────────────────────────────

bool InstallWorker::stepServices() {
    step("Enabling system services", 60);
    return sh({
        "arch-chroot /mnt systemctl enable sddm",
        "arch-chroot /mnt systemctl enable NetworkManager",
        "arch-chroot /mnt systemctl enable bluetooth",
        "arch-chroot /mnt systemctl enable power-profiles-daemon",
        "arch-chroot /mnt systemctl enable fstrim.timer",
        // Copy live WiFi connections
        "mkdir -p /mnt/etc/NetworkManager/system-connections",
        "cp -a /etc/NetworkManager/system-connections/. /mnt/etc/NetworkManager/system-connections/ 2>/dev/null || true",
        "chmod 600 /mnt/etc/NetworkManager/system-connections/* 2>/dev/null || true",
        // SDDM Breeze
        "mkdir -p /mnt/etc/sddm.conf.d",
        "printf '[Theme]\\nCurrent=breeze\\n' > /mnt/etc/sddm.conf.d/theme.conf",
        // Move X11 Plasma session to backup (Wayland default)
        "[ -f /mnt/usr/share/xsessions/plasma.desktop ] && "
        "mv /mnt/usr/share/xsessions/plasma.desktop /mnt/usr/share/xsessions/plasma.desktop.bak || true",
        // Mask plasma-krunner
        "mkdir -p /mnt/etc/systemd/user",
        "ln -sf /dev/null /mnt/etc/systemd/user/plasma-krunner.service || true",
        // Pacman cache cleanup hook
        "mkdir -p /mnt/etc/pacman.d/hooks",
        "printf '[Trigger]\\nOperation=Upgrade\\nOperation=Install\\nOperation=Remove\\n"
        "Type=Package\\nTarget=*\\n[Action]\\nDescription=Cleaning pacman cache...\\n"
        "When=PostTransaction\\nExec=/usr/bin/paccache -rk0\\n' > /mnt/etc/pacman.d/hooks/clean_cache.hook",
    }, "services");
}

// ── STEP 13 — DNS-over-TLS ────────────────────────────────────────────────

bool InstallWorker::stepDNS() {
    step("Configuring DNS-over-TLS", 62);
    if (m_cfg.dnsProvider == "none") return true;

    QString dns1, dns2, dnsHost;
    if (m_cfg.dnsProvider == "cloudflare") {
        dns1 = "1.1.1.1"; dns2 = "1.0.0.1"; dnsHost = "cloudflare-dns.com";
    } else if (m_cfg.dnsProvider == "adguard") {
        dns1 = "94.140.14.14"; dns2 = "94.140.15.15"; dnsHost = "dns.adguard.com";
    } else if (m_cfg.dnsProvider == "quad9") {
        dns1 = "9.9.9.9"; dns2 = "149.112.112.112"; dnsHost = "dns.quad9.net";
    }

    return sh({
        "arch-chroot /mnt systemctl enable systemd-resolved",
        "mkdir -p /mnt/etc/systemd/resolved.conf.d",
        QString("printf '[Resolve]\\nDNS=%1#%3 %2#%3\\nDNSOverTLS=yes\\nDNSSEC=yes\\n' "
                "> /mnt/etc/systemd/resolved.conf.d/gotunos.conf").arg(dns1, dns2, dnsHost),
        "ln -sf /run/systemd/resolve/stub-resolv.conf /mnt/etc/resolv.conf",
        // NM: stop NM from managing DNS so systemd-resolved handles it
        "mkdir -p /mnt/etc/NetworkManager/conf.d",
        "printf '[main]\\ndns=systemd-resolved\\n' > /mnt/etc/NetworkManager/conf.d/dns.conf",
    }, "dns");
}

// ── STEP 14 — ZRAM ────────────────────────────────────────────────────────

bool InstallWorker::stepZRAM() {
    step("Configuring ZRAM swap", 64);
    if (!m_cfg.zramEnabled) return true;

    QString size = (m_cfg.zramSizeMiB == 0) ? "ram/2" : QString("%1MiB").arg(m_cfg.zramSizeMiB);

    return sh({
        "arch-chroot /mnt pacman -S --noconfirm zram-generator",
        QString("printf '[zram0]\\nzram-size = %1\\ncompression-algorithm = zstd\\n' "
                "> /mnt/etc/systemd/zram-generator.conf").arg(size),
    }, "zram");
}

// ── STEP 15 — Secondary drives ────────────────────────────────────────────

bool InstallWorker::stepSecondaryDrives() {
    step("Mounting secondary drives", 66);
    if (m_cfg.secondaryDrives.isEmpty()) return true;

    QStringList s;
    s << "set -e";

    for (const auto &drv : m_cfg.secondaryDrives) {
        // Get UUID
        QString uuid = QString("$(blkid -o value -s UUID %1)").arg(drv.partition);
        // Strip leading / so path is relative
        QString mntRel = drv.mountPoint.startsWith('/') ? drv.mountPoint.mid(1) : drv.mountPoint;
        QString mntAbs = "/mnt/" + mntRel;

        s << QString("mkdir -p %1").arg(mntAbs);

        // Mount based on fstype
        if (drv.fsType == "ntfs" || drv.fsType == "vfat" || drv.fsType == "exfat") {
            s << QString("mount -o uid=1000,gid=1000,rw,umask=0000,dmask=0000,fmask=0000 %1 %2 || true")
                     .arg(drv.partition, mntAbs);
            s << QString("chmod 777 %1 2>/dev/null || true").arg(mntAbs);
            // Fstab entry with nofail + proper Steam Proton permissions
            s << QString("UUID=%2 /%3 %4 uid=1000,gid=1000,rw,user,exec,umask=0000,dmask=0000,"
                         "fmask=0000,nofail,x-systemd.device-timeout=5s 0 0")
                     .prepend("echo \"").append("\" >> /mnt/etc/fstab")
                    .replace("UUID=", "UUID=")
                    ; // rebuild cleanly:
            // Use a clean echo command instead
            s << QString("echo 'UUID=%1 /%2 %3 uid=1000,gid=1000,rw,user,exec,umask=0000,dmask=0000,"
                         "fmask=0000,nofail,x-systemd.device-timeout=5s 0 0' >> /mnt/etc/fstab")
                     .arg(uuid, mntRel, drv.fsType);
        } else {
            s << QString("mount %1 %2 || true").arg(drv.partition, mntAbs);
            s << QString("chmod 777 %1").arg(mntAbs);
            s << QString("chown -R 1000:1000 %1").arg(mntAbs);
            s << QString("echo 'UUID=%1 /%2 %3 defaults,exec,rw,nofail,x-systemd.device-timeout=5s 0 2' >> /mnt/etc/fstab")
                     .arg(uuid, mntRel, drv.fsType.isEmpty() ? "auto" : drv.fsType);
        }

        // Symlinks per user
        if (!drv.symlinkName.isEmpty()) {
            for (const auto &u : m_cfg.users) {
                s << QString("ln -sf /%1 /mnt/home/%2/%3").arg(mntRel, u.username, drv.symlinkName);
                s << QString("chown -h 1000:1000 /mnt/home/%1/%2").arg(u.username, drv.symlinkName);
            }
        }
    }

    return sh(s, "secondary");
}

// ── STEP 16 — AUR ────────────────────────────────────────────────────────

bool InstallWorker::stepAUR() {
    step("Building yay + installing AUR packages", 68);
    if (m_cfg.users.isEmpty()) return true;
    const QString u1 = m_cfg.users.first().username;

    QStringList aur = {
        "ark", "atlauncher-bin", "bazaar", "shelly-bin", "rs-pug-git",
        "obsidian-bin", "vesktop-bin", "zen-browser-bin", "coolercontrol-bin",
        "filelight", "krita", "goverlay", "protonplus", "protontricks",
        "faugus-launcher", "kitty", "kate", "kdeconnect", "localsend-bin",
        "gparted", "popsicle-bin", "hardinfo2", "okular", "openrgb",
        "proton-vpn-gtk-app", "fastfetch", "heroic-games-launcher-bin",
        "steam", "onlyoffice-bin", "winboat-bin", "easyeffects", "gamemode",
        "zapzap", "waydroid", "kcalc", "thunderbird", "flatseal",
        "sunshine-bin", "supertuxkart", "bleachbit", "stacer-bin", "fuse2",
        "hydra-launcher-bin", "woeusb", "input-remapper", "arch-update",
        "lutris", "jq", "piper", "mousam",
        "gnome-boxes",
        "plasma6-wallpapers-smart-video-wallpaper-reborn",
        "wallpaper-engine-kde-plugin-git", "optiscaler-client",
        "fresh-editor-bin", "furmark2-bin",
    };

    if (m_cfg.gpuDriver == "amd")               aur << "lact";
    if (m_cfg.gpuDriver.startsWith("nvidia"))   aur << "greenwithenvy";
    if (m_cfg.razer)                             aur << "openrazer-driver-dkms" << "polychromatic";
    if (m_cfg.filesystem == "btrfs")             aur << "btrfs-assistant" << "timeshift";

    QStringList s;
    // Temp sudoers for AUR builds
    s << QString("echo '%1 ALL=(ALL) NOPASSWD: ALL' > /mnt/etc/sudoers.d/aur_temp").arg(u1);
    s << "chmod 440 /mnt/etc/sudoers.d/aur_temp";

    // Build yay
    s << QString("arch-chroot /mnt /bin/bash -c '"
                 "cd /tmp && rm -rf yay-bin && "
                 "git clone --depth 1 https://aur.archlinux.org/yay-bin.git && "
                 "chown -R %1:%1 yay-bin && "
                 "cd yay-bin && sudo -u %1 makepkg -si --noconfirm'").arg(u1);

    // Install AUR packages one by one (continue on failure)
    for (const auto &pkg : aur) {
        s << QString("arch-chroot /mnt sudo -u %1 yay -S --noconfirm --overwrite '*' %2 || "
                     "echo 'WARN: AUR package %2 failed — continuing'").arg(u1, pkg);
    }

    // Clean up temp sudoers
    s << "rm -f /mnt/etc/sudoers.d/aur_temp";

    // Enable services
    if (m_cfg.gpuDriver == "amd")
        s << "arch-chroot /mnt systemctl enable lactd || true";
    s << "arch-chroot /mnt systemctl enable ratbagd || true";

    if (m_cfg.razer) {
        for (const auto &u : m_cfg.users)
            s << QString("arch-chroot /mnt gpasswd -a %1 openrazer || true").arg(u.username);
    }

    return sh(s, "aur");
}

// ── STEP 17 — Flatpak ─────────────────────────────────────────────────────

bool InstallWorker::stepFlatpak() {
    step("Installing Flatpak apps", 86);
    return sh({
        "arch-chroot /mnt flatpak remote-add --if-not-exists flathub "
        "https://dl.flathub.org/repo/flathub.flatpakrepo",
        "arch-chroot /mnt flatpak install -y flathub "
        "rocks.shy.VacuumTube "
        "com.dec05eba.gpu_screen_recorder "
        "me.proton.Pass "
        "com.rafaelmardojai.Blanket "
        "com.spotify.Client "
        "net.retrodeck.retrodeck "
        "net.shadps4.shadPS4 "
        "com.stremio.Stremio",
    }, "flatpak");
}

// ── STEP 18 — KDE config ─────────────────────────────────────────────────

bool InstallWorker::stepKDEConfig() {
    step("Configuring KDE Plasma & per-user setup", 91);

    QStringList s;

    // Remove unwanted packages
    for (const auto &p : QStringList{"plasma-welcome","malcontent","discover","micro",
                                     "winetricks","stacer","scrcpy","pavucontrol"})
        s << QString("arch-chroot /mnt pacman -Rdd --noconfirm %1 2>/dev/null || true").arg(p);

    // Per-user
    for (const auto &u : m_cfg.users) {
        QString home = QString("/mnt/home/%1").arg(u.username);

        // Steam Gamepad UI autostart
        s << QString("mkdir -p %1/.config/autostart").arg(home);
        s << QString("printf '[Desktop Entry]\\nName=Steam Gamepad UI\\nExec=steam -gamepadui\\n"
                     "Icon=steam\\nTerminal=false\\nType=Application\\nCategories=Game;\\n' "
                     "> %1/.config/autostart/steam-gamepadui.desktop").arg(home);

        // kdeglobals Breeze Dark
        s << QString("mkdir -p %1/.config").arg(home);
        s << QString("printf '[General]\\nColorScheme=BreezeDark\\n"
                     "[KDE]\\nLookAndFeelPackage=org.kde.breezedark.desktop\\nSingleClick=false\\n' "
                     "> %1/.config/kdeglobals").arg(home);

        // oh-my-zsh (non-interactive)
        s << QString("arch-chroot /mnt sudo -u %1 sh -c 'HOME=/home/%1 RUNZSH=no CHSH=no "
                     "sh -c \"$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)\"' "
                     "|| true").arg(u.username);
        s << QString("echo 'fastfetch' >> %1/.zshrc").arg(home);
        s << QString("chown -R %1:%1 %2").arg(u.username, home);
    }

    // root oh-my-zsh
    s << "arch-chroot /mnt sh -c 'HOME=/root RUNZSH=no CHSH=no "
         "sh -c \"$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)\"' || true";
    s << "echo 'fastfetch' >> /mnt/root/.zshrc";

    return sh(s, "kde");
}

// ── STEP 19 — First-boot service ──────────────────────────────────────────

bool InstallWorker::stepFirstBoot() {
    step("Installing first-boot Decky service", 96);
    if (m_cfg.users.isEmpty()) return true;
    const QString user1 = m_cfg.users.first().username;

    QStringList s;
    s << "mkdir -p /mnt/usr/local/bin";

    // Write the firstboot script (matches TUI logic exactly: state file + pluginloader retry)
    s << QString(
        "cat > /mnt/usr/local/bin/gotunos-firstboot.sh << 'SCRIPT'\n"
        "#!/usr/bin/env bash\n"
        "GAMING_USER=\"%1\"\n"
        "STATE=/var/lib/gotunos-firstboot.state\n"
        "notify() {\n"
        "    uid=$(id -u \"$GAMING_USER\" 2>/dev/null) || return\n"
        "    [ -S /run/user/${uid}/bus ] && \\\n"
        "        sudo -u \"$GAMING_USER\" DBUS_SESSION_BUS_ADDRESS=\"unix:path=/run/user/${uid}/bus\" \\\n"
        "        notify-send \"GötünOS Setup\" \"$1\" 2>/dev/null || true\n"
        "}\n"
        "powerprofilesctl set performance 2>/dev/null || true\n"
        "if [ ! -f \"$STATE\" ]; then\n"
        "    echo 1 > \"$STATE\"\n"
        "    (\n"
        "      for i in $(seq 1 30); do\n"
        "          uid=$(id -u \"$GAMING_USER\" 2>/dev/null)\n"
        "          if [ -n \"$uid\" ] && [ -S \"/run/user/${uid}/bus\" ]; then break; fi\n"
        "          sleep 2\n"
        "      done\n"
        "      uid=$(id -u \"$GAMING_USER\" 2>/dev/null)\n"
        "      if [ -n \"$uid\" ] && [ -S \"/run/user/${uid}/bus\" ]; then\n"
        "          sudo -u \"$GAMING_USER\" DBUS_SESSION_BUS_ADDRESS=\"unix:path=/run/user/${uid}/bus\" \\\n"
        "              lookandfeeltool -a org.kde.breezedark.desktop 2>/dev/null || true\n"
        "      fi\n"
        "      sleep 240\n"
        "      notify \"Downloading Decky Loader...\"\n"
        "      wget -O /tmp/decky.sh https://github.com/SteamDeckHomebrew/decky-installer/releases/latest/download/install_release.sh\n"
        "      export SUDO_USER=\"$GAMING_USER\"\n"
        "      if sh /tmp/decky.sh; then\n"
        "        notify \"Decky Loader installed!\"\n"
        "      else\n"
        "        notify \"Decky install failed - will retry next boot.\"\n"
        "      fi\n"
        "      rm -f /tmp/decky.sh\n"
        "    ) &\n"
        "else\n"
        "    if systemctl is-active --quiet pluginloader; then\n"
        "        systemctl disable gotunos-firstboot.service 2>/dev/null\n"
        "        rm -f /etc/systemd/system/gotunos-firstboot.service \"$STATE\" \"$0\"\n"
        "    else\n"
        "        ( sleep 60\n"
        "          wget -O /tmp/decky.sh https://github.com/SteamDeckHomebrew/decky-installer/releases/latest/download/install_release.sh\n"
        "          export SUDO_USER=\"$GAMING_USER\"\n"
        "          if sh /tmp/decky.sh; then\n"
        "              notify \"Decky Loader installed!\"\n"
        "              systemctl disable gotunos-firstboot.service 2>/dev/null\n"
        "              rm -f /etc/systemd/system/gotunos-firstboot.service \"$STATE\" \"$0\"\n"
        "          fi\n"
        "          rm -f /tmp/decky.sh ) &\n"
        "    fi\n"
        "fi\n"
        "SCRIPT").arg(user1);

    s << "chmod +x /mnt/usr/local/bin/gotunos-firstboot.sh";

    // Systemd unit
    s << "cat > /mnt/etc/systemd/system/gotunos-firstboot.service << 'EOF'\n"
          "[Unit]\n"
          "Description=GötünOS First Boot Setup\n"
          "After=network-online.target graphical.target\n"
          "Wants=network-online.target graphical.target\n"
          "[Service]\n"
          "Type=oneshot\n"
          "RemainAfterExit=yes\n"
          "ExecStart=/usr/local/bin/gotunos-firstboot.sh\n"
          "[Install]\n"
          "WantedBy=graphical.target\n"
          "EOF";

    s << "arch-chroot /mnt systemctl enable gotunos-firstboot.service || true";
    return sh(s, "firstboot");
}

// ── STEP 20 — Branding ───────────────────────────────────────────────────

bool InstallWorker::stepBranding() {
    step("Applying GötünOS branding", 98);
    return sh({
        "printf 'NAME=\"GötünOS\"\\nID=gotunos\\nID_LIKE=arch\\nPRETTY_NAME=\"GötünOS\"\\nANSI_COLOR=\"0;36\"\\nHOME_URL=\"https://gotunos.org\"\\n' "
        "> /mnt/etc/os-release",
        "printf 'DISTRIB_ID=GötünOS\\nDISTRIB_RELEASE=12\\nDISTRIB_DESCRIPTION=\"GötünOS\"\\n' > /mnt/etc/lsb-release",
        "mkdir -p /mnt/usr/share/gotunos-installer",
        "cat > /mnt/usr/share/gotunos-installer/rebrand.sh <<'EOF'\n"
        "#!/bin/sh\n"
        "cat > /etc/os-release <<'INNER'\n"
        "NAME=\"GötünOS\"\n"
        "ID=gotunos\n"
        "ID_LIKE=arch\n"
        "PRETTY_NAME=\"GötünOS\"\n"
        "ANSI_COLOR=\"0;36\"\n"
        "HOME_URL=\"https://gotunos.org\"\n"
        "INNER\n"
        "EOF",
        "chmod +x /mnt/usr/share/gotunos-installer/rebrand.sh",
        "mkdir -p /mnt/etc/pacman.d/hooks",
        "cat > /mnt/etc/pacman.d/hooks/gotunos-branding.hook <<'EOF'\n"
        "[Trigger]\n"
        "Operation = Install\n"
        "Operation = Upgrade\n"
        "Type = Package\n"
        "Target = filesystem\n"
        "[Action]\n"
        "Description = Re-applying GötünOS branding...\n"
        "When = PostTransaction\n"
        "Exec = /usr/share/gotunos-installer/rebrand.sh\n"
        "EOF",
        "cp /var/log/gotunos-install.log /mnt/var/log/gotunos_install.log 2>/dev/null || true",
    }, "brand");
}

// ── constructor ──────────────────────────────────────────────────────────
InstallWorker::InstallWorker(const InstallConfig &cfg, QObject *parent)
    : QThread(parent), m_cfg(cfg) {}

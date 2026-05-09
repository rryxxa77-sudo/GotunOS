# GötünOS

> A post-install app for EndeavourOS that transforms a base installation into a fully featured KDE Plasma desktop environment — with curated applications, gaming tools, custom themes, and system-level tweaks applied automatically.

> **Fun fact:** GötünOS is in Turkish and  roughly translates to "YourAssOS" in English. You're welcome.

---

## ⚠️ Requirements

- **EndeavourOS must be installed before running this script.**
- The script will fail on stock Arch Linux or other Arch-based distributions.
- An active internet connection is required throughout the installation.
- `yay` AUR helper must be available.

---

## What This Script Does

| Category | Action |
|---|---|
| Package installation | Installs packages from AUR and Flatpak |
| Shell | Switches the default shell from Bash to Fish |
| Branding | Renames the OS and configures Fastfetch on terminal startup |
| Theming | Applies 4 custom KDE themes by MurderFromMars |
| Tooling | Replaces EndeavourOS-specific tools with CachyOS equivalents |
| Power | Sets `power-profiles-daemon` to Performance mode |
| Bluetooth | Enables and starts the Bluetooth service |
| Installer | Provides a Windows-style graphical installer experience |

---

## Packages Installed

### From AUR

#### Archiving & File Management
| Package | Description |
|---|---|
| `ark` | Archive manager for KDE |
| `filelight` | Disk usage visualizer |
| `yazi` | Terminal file manager |
| `gparted` | Partition editor |
| `fuse2` | Filesystem in Userspace support, required by some AppImages |

#### Terminal & Shell Utilities
| Package | Description |
|---|---|
| `kitty` | GPU-accelerated terminal emulator |
| `fish` | User-friendly interactive shell |
| `micro` | Modern terminal text editor |
| `fastfetch` | System information display tool |
| `btop` | Resource monitor |
| `cava` | Console audio visualizer |
| `cmatrix` | Matrix-style terminal animation |
| `hollywood` | Fills your terminal with fictional hacker activity |
| `sl` | A steam locomotive that runs across your terminal when you mistype `ls` |
| `thefuck` | Automatically corrects your previous console command |
| `ttf-jetbrains-mono-nerd` | JetBrains Mono font patched with Nerd Font icons |

#### Text Editors & Office
| Package | Description |
|---|---|
| `kate` | Advanced KDE text editor |
| `onlyoffice` | Full-featured office suite |
| `okular` | Document viewer |

#### Gaming
| Package | Description |
|---|---|
| `steam` | Valve's game distribution platform |
| `heroic-launcher` | Epic Games and GOG launcher for Linux |
| `faugus-launcher` | Feature-rich game launcher with support for Epic, Rockstar, GOG, EA, Battle.net, and more — running launchers and games as if they were installed natively |
| `hydra-launcher` | Game launcher with a built-in torrent client |
| `atlauncher` | Minecraft launcher |
| `supertuxkart` | Open-source kart racing game |
| `mangohud` | In-game performance overlay |
| `goverlay` | Graphical configuration tool for MangoHud |
| `gamemode` | Optimizes system performance while gaming |
| `protonplus` | Proton and Wine version manager |
| `protontricks` | Winetricks wrapper for Steam games |
| `winboat` | Windows container for running Windows applications |

#### System & Hardware
| Package | Description |
|---|---|
| `hardinfo2` | System information and benchmark tool |
| `coolercontrol` | Fan and cooling control application |
| `lact` | Linux GPU control utility for AMD and NVIDIA |
| `openrgb` | RGB lighting control for all your peripherals |
| `stacer` | System optimizer and monitor |
| `bleachbit` | System cleaner and privacy tool |
| `appimagelauncher` | AppImage integration helper |
| `flatpak` | Flatpak runtime and CLI |

#### Audio & Video
| Package | Description |
|---|---|
| `vlc` | Versatile media player |
| `easyeffects` | Audio effects and equalizer for PipeWire |
| `pavucontrol` | PulseAudio volume control panel |

#### Networking & Remote
| Package | Description |
|---|---|
| `protonvpn-gtk-app` | ProtonVPN graphical client |
| `tailscale` | Mesh VPN service |
| `kdeconnect` | Seamless integration between your phone and KDE desktop |
| `localsend` | Cross-device local file sharing |
| `scrcpy` | Android screen mirroring and control |
| `waydroid` | Android container for Linux |
| `sunshine` | Self-hosted game streaming server |
| `moonlight` | Game streaming client for Sunshine and GeForce Experience |

#### KDE Plasma Components
| Package | Description |
|---|---|
| `plasma-nm` | Network Manager applet for KDE Plasma |
| `plasma-pa` | PulseAudio volume applet for KDE Plasma |
| `bluez` | Bluetooth protocol stack |
| `bluedevil` | KDE Bluetooth integration |

#### Productivity & Communication
| Package | Description |
|---|---|
| `thunderbird` | Email client by Mozilla |
| `zapzap` | WhatsApp desktop client for Linux |
| `obsidian` | Markdown-based note-taking and knowledge management app |
| `vesktop` | Discord client with Vencord enhancements built in |
| `zen-browser` | Privacy-focused browser based on Firefox |
| `shelly` | Graphical package manager |

#### Image & Graphics
| Package | Description |
|---|---|
| `krita` | Professional digital painting application |
| `upscayl` | AI-powered image upscaler |
| `popsicle` | Fast and simple USB flashing tool |

#### Miscellaneous
| Package | Description |
|---|---|
| `rs-pug` | YouTube Music client in the terminal (TUI) |
| `kcalc` | KDE calculator |

---

### From Flatpak

| Package | Description |
|---|---|
| `Mission Center` | System resource monitor |
| `Trinity Launcher` | Minecraft Bedrock launcher for Linux |
| `Mousam` | Weather application |
| `Spotify` | Music streaming client |
| `VacuumTube` | YouTube client designed for TV-style viewing on Linux |
| `GPU Screen Recorder` | Low-overhead, GPU-accelerated screen recorder.And the best recorder ever made. |
| `Bitwarden` | Open-source password manager |
| `Blanket` | Ambient sound player for focus and relaxation |
| `Flatseal` | Graphical Flatpak permissions manager |

---

## KDE Themes

Four custom KDE Plasma 6 themes by **MurderFromMars** are included. Preview screenshots of all themes are available in the installer.

| Theme | Description |
|---|---|
| **NovaChoom** | A neon-red, cyberpunk-inspired theme with high-contrast crimson visuals and a dynamic tiling window manager layout focused on productivity. |
| **PS4** | A PlayStation 4-inspired theme featuring the signature cobalt blue aesthetic, adapted for a tiling workflow on KDE Plasma 6. |
| **LunarGlass** | A frosted glass, space-themed aesthetic with translucent visuals and a polished tiling window manager setup. |
| **Carnage** | A bold, aggressive theme from MurderFromMars — not for the faint of heart. |

---

## System Configuration

- **Shell:** Bash → Fish
- **Fastfetch:** Enabled on terminal startup
- **OS branding:** Renamed to GötünOS
- **EndeavourOS tools:** Replaced with CachyOS equivalents
- **Power profile:** Set to Performance via `power-profiles-daemon`
- **Bluetooth:** Enabled and started via `systemctl`

---

## Installation

> Ensure EndeavourOS is fully installed before proceeding.

Download the `.tar.zst` package and install it with pacman:

```bash
sudo pacman -U /home/yourusername/locationitisin/GotunOS.tar.zst
```

Then launch the graphical installer and follow the on-screen steps.

---

## Credits

- Themes by **MurderFromMars** — https://github.com/MurderFromMars
- Built on top of **EndeavourOS** and the **KDE Plasma** desktop

## Source Code
-Source Code is avaible on releases

# GötünOS

GötünOS is a gaming focused Arch Linux distribution with KDE Plasma 6 on top.
This repo holds the installer that runs on the live ISO: a Qt6/C++ wizard that
sets up disks, users, drivers and all the gaming bits in one go.

It started as a bash/dialog TUI (still in `legacy/installer.sh` if you want to
see where it came from) and is now a proper graphical installer.

> Beta. It partitions and formats real disks, so please try it in a VM first.

## What the installer does

- English and Turkish, switchable on the first page
- Wi-Fi and ethernet setup through NetworkManager, plus a mirror refresh with reflector
- Encrypted DNS (Cloudflare, AdGuard, Quad9 or your own servers), or leave DNS alone
- Locale, timezone and console keymap, with the keymap applied live so you can test it
- As many user accounts as you want, optional shared root password, optional passwordless sudo
- Hardware autodetection: AMD / NVIDIA (open or legacy) / Intel drivers, CPU microcode, Razer gear
- Four disk modes: wipe and install, manual with cfdisk, replace an existing root partition, or grow a
  too small EFI partition
- ext4, btrfs or xfs, with systemd-boot or GRUB
- Extra drives mounted with Steam friendly permissions and optional `~/Games` style symlinks
- ZRAM and/or a swap file, hostname and domain
- A summary page before anything is written, then pacstrap, KDE, services, AUR (yay), Flatpak and
  branding, with a live log at `/var/log/gotunos-install.log`

## Layout

```
src/            the Qt6 installer
  pages/        one file per wizard page
  backend/      config struct, translations, the install worker
  widgets/      shared widgets (the searchable combo box)
assets/         stylesheet + Qt resources
packaging/      desktop entry for the live session
legacy/         the old TUI installer, kept for reference only
```

## Building

You need Qt6 (Widgets), CMake 3.21+ and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
./build/gotunos-installer-gui
```

On Arch: `sudo pacman -S --needed base-devel cmake qt6-base`.

Without root it starts in a look-around mode and warns you; the actual install
needs `sudo`, `arch-chroot`, `pacstrap` and friends, so it only really works
from the live ISO.

## Contributing

Issues and PRs are welcome. Keep changes small and try to build before pushing.
If you touch a wizard page, remember both languages live in
`src/backend/I18n.h`.

## License

GPL-3.0-or-later, see [LICENSE](LICENSE).

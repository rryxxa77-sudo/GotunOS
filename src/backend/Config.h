#pragma once
#include <QString>
#include <QStringList>
#include <QList>

struct SecondaryDrive {
    QString partition;   // e.g. /dev/sdb1
    QString mountPoint;  // e.g. /mnt/data
    QString symlinkName; // e.g. Games  (empty = no symlink)
    QString fsType;      // detected by blkid
};

struct UserEntry {
    QString username;
    QString password;
};

struct InstallConfig {
    // ── Language & Locale ─────────────────────
    QString language   = "en";
    QString locale     = "en_US.UTF-8";
    QString timezone   = "Europe/Istanbul";
    QString keymap     = "us";

    // ── Network / DNS ──────────────────────────
    QString dnsProvider = "cloudflare"; // cloudflare, adguard, quad9, none
    QString dnsIp1, dnsIp2, dnsHost;

    // ── Users ──────────────────────────────────
    QList<UserEntry> users;
    QString          rootPassword;
    bool             passwordlessSudo = false;

    // ── Hardware ───────────────────────────────
    QString gpuDriver = "none"; // amd, nvidia-open, nvidia-legacy, intel, none
    QString cpuType   = "amd"; // amd, intel
    bool    razer     = false;

    // ── Disk ───────────────────────────────────
    QString targetDisk;
    QString efiPart;   // only used in manual/replace modes
    QString rootPart;  // only used in manual/replace modes
    QString partMode   = "wipe"; // wipe, manual, replace, grow
    QString filesystem = "ext4"; // ext4, xfs, btrfs
    QString bootloader = "systemd-boot"; // systemd-boot, grub

    // ── Secondary Drives ───────────────────────
    QList<SecondaryDrive> secondaryDrives;

    // ── Options ────────────────────────────────
    bool    zramEnabled = true;
    int     zramSizeMiB = 0;    // 0 = auto (ram/2)
    bool    swapEnabled = false;
    int     swapSizeGiB = 4;
    QString hostname    = "gotunos";
    QString domain      = "localdomain";
};

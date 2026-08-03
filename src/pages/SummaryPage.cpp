#include "SummaryPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

SummaryPage::SummaryPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(14);

    auto *tt = new QLabel(I18n::tr("summary_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("summary_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(4);

    auto *warn = new QLabel(I18n::language() == "tr" ?
        "⚠  Gözden geçirmek için son şansınız. Şimdi Kur'a tıklamak seçilen diski BİÇİMLENDİRECEK ve GötünOS'u KURACAKTIR." :
        "⚠  This is your last chance to review. Clicking Install Now will FORMAT and INSTALL GötünOS on the selected disk."
    );
    warn->setWordWrap(true);
    warn->setObjectName("cardWarn");
    warn->setStyleSheet("padding:12px 16px; color:#ff9500; font-size:12px; font-weight:bold;");
    lay->addWidget(warn);

    m_text = new QTextEdit();
    m_text->setReadOnly(true);
    m_text->setFont(QFont("SF Mono, JetBrains Mono, Cascadia Code, monospace", 11));
    m_text->setStyleSheet("background-color:#1c1c1e; color:#30d158; border:1px solid #d1d1d6; border-radius:8px;");
    lay->addWidget(m_text, 1);
}

void SummaryPage::onShow() {
    bool tr = (I18n::language() == "tr");
    QString s;

    s += tr ? "╔══════════════════════════════════════════════╗\n" : "╔══════════════════════════════════════════════╗\n";
    s += tr ? "║          GötünOS Kurulum Planı               ║\n" : "║          GötünOS Installation Plan          ║\n";
    s += tr ? "╚══════════════════════════════════════════════╝\n\n" : "╚══════════════════════════════════════════════╝\n\n";

    s += tr ? "── DİL & BÖLGE ─────────────────────────────────\n" : "── LOCALE ─────────────────────────────────────\n";
    s += QString("  Language / Dil  %1\n").arg(m_cfg->language == "tr" ? "Türkçe" : "English");
    s += QString("  Locale          %1\n").arg(m_cfg->locale);
    s += QString("  Timezone        %1\n").arg(m_cfg->timezone);
    s += QString("  Keymap          %1\n\n").arg(m_cfg->keymap);

    s += tr ? "── AĞ ──────────────────────────────────────────\n" : "── NETWORK ─────────────────────────────────────\n";
    s += QString("  DNS Provider    %1\n").arg(m_cfg->dnsProvider);
    if (m_cfg->dnsProvider == "custom")
        s += QString("  DNS Servers     %1 %2 %3\n")
                 .arg(m_cfg->dnsIp1, m_cfg->dnsIp2,
                      m_cfg->dnsHost.isEmpty() ? "(no DoT hostname)" : m_cfg->dnsHost);
    s += "\n";

    s += tr ? "── KULLANICILAR ────────────────────────────────\n" : "── USERS ───────────────────────────────────────\n";
    for (const auto &u : m_cfg->users)
        s += QString("  User            %1\n").arg(u.username);
    s += QString("  Passwordless    %1\n\n")
             .arg(m_cfg->passwordlessSudo ? "Yes (wheel)" : "No");

    s += tr ? "── DONANIM ─────────────────────────────────────\n" : "── HARDWARE ────────────────────────────────────\n";
    s += QString("  GPU Driver      %1\n").arg(m_cfg->gpuDriver);
    s += QString("  CPU             %1\n").arg(m_cfg->cpuType);
    s += QString("  Razer Support   %1\n\n").arg(m_cfg->razer ? "Yes" : "No");

    s += tr ? "── DİSK BÖLÜMLEME ──────────────────────────────\n" : "── DISK ────────────────────────────────────────\n";
    s += QString("  Target Disk     %1\n").arg(m_cfg->targetDisk);
    s += QString("  Partition Mode  %1\n").arg(m_cfg->partMode);
    if (m_cfg->partMode != "wipe") {
        s += QString("  EFI Partition   %1\n").arg(m_cfg->efiPart);
        s += QString("  Root Partition  %1\n").arg(m_cfg->rootPart);
    }
    s += QString("  Filesystem      %1\n").arg(m_cfg->filesystem);
    s += QString("  Bootloader      %1\n\n").arg(m_cfg->bootloader);

    if (!m_cfg->secondaryDrives.isEmpty()) {
        s += tr ? "── İKİNCİL SÜRÜCÜLER ───────────────────────────\n" : "── SECONDARY DRIVES ────────────────────────────\n";
        for (const auto &drv : m_cfg->secondaryDrives) {
            s += QString("  %1  →  %2").arg(drv.partition, drv.mountPoint);
            if (!drv.symlinkName.isEmpty())
                s += QString("  (symlinked as ~/%1)").arg(drv.symlinkName);
            s += "\n";
        }
        s += "\n";
    }

    s += tr ? "── SİSTEM SEÇENEKLERİ ──────────────────────────\n" : "── OPTIONS ─────────────────────────────────────\n";
    s += QString("  Hostname        %1.%2\n").arg(m_cfg->hostname, m_cfg->domain);
    s += QString("  ZRAM            %1\n")
             .arg(m_cfg->zramEnabled ? (m_cfg->zramSizeMiB == 0 ? "Auto (RAM/2)" :
                  QString("%1 GiB").arg(m_cfg->zramSizeMiB/1024)) : "Disabled");
    s += QString("  Disk Swap       %1\n\n")
             .arg(m_cfg->swapEnabled ? QString("%1 GiB").arg(m_cfg->swapSizeGiB) : "Disabled");

    s += tr ? "── PAKETLER ────────────────────────────────────\n" : "── PACKAGES ────────────────────────────────────\n";
    s += "  KDE Plasma + base-devel + linux-zen + linux-lts\n";
    s += "  jq + flatpak + pipewire + plymouth + zsh + oh-my-zsh\n";
    s += "  AUR: yay, steam, heroic, lutris, arch-update, vesktop,\n";
    s += "       zen-browser, coolercontrol, obsidian, kitty, kate ...\n";
    s += "  Flatpak: gpu_screen_recorder, Spotify, Stremio, RetroDECK ...\n";

    m_text->setPlainText(s);
}

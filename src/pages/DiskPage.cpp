#include "DiskPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QRadioButton>
#include <QGroupBox>
#include <QFrame>
#include <QScrollArea>
#include <QMessageBox>
#include <QRegularExpression>

DiskPage::DiskPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 32, 52, 16);
    lay->setSpacing(12);

    auto *tt = new QLabel(I18n::tr("disk_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("disk_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(2);

    // ── Disk picker ───────────────────────────────────────────────────────────
    auto *dl = new QLabel(I18n::tr("disks_header")); dl->setObjectName("sectionHeader");
    lay->addWidget(dl);
    m_diskList = new QListWidget(); m_diskList->setFixedHeight(100);
    lay->addWidget(m_diskList);

    // ── Partition mode buttons ────────────────────────────────────────────────
    auto *ml = new QLabel(I18n::tr("mode_header")); ml->setObjectName("sectionHeader");
    lay->addWidget(ml);

    auto *mrow = new QHBoxLayout(); mrow->setSpacing(8);
    m_modeGroup = new QButtonGroup(this);
    const QStringList modeLabels = I18n::language() == "tr" ?
        QStringList{"Diski Temizle ve Kur", "Elle Bölümleme (cfdisk)", "Bölümü Değiştir", "EFI Büyüt"} :
        QStringList{"Wipe & Install", "Manual (cfdisk)", "Replace Partition", "Grow EFI"};

    for (int i = 0; i < modeLabels.size(); i++) {
        auto *rb = new QRadioButton(modeLabels[i]);
        if (i == 0) rb->setChecked(true);
        m_modeGroup->addButton(rb, i);
        mrow->addWidget(rb);
    }
    mrow->addStretch();
    lay->addLayout(mrow);

    m_warnLbl = new QLabel(I18n::language() == "tr" ? "⚠  Temizleme modu seçilen diskteki TÜM verileri silecektir." : "⚠  Wipe mode will erase ALL data on the selected disk.");
    m_warnLbl->setObjectName("statusWarn");
    lay->addWidget(m_warnLbl);

    // ── Mode-specific stack ───────────────────────────────────────────────────
    m_modeStack = new QStackedWidget();

    // Page 0 — Wipe
    auto *wipePage = new QWidget();
    auto *wpLay = new QVBoxLayout(wipePage);
    wpLay->setContentsMargins(0,0,0,0);
    auto *wipeInfo = new QLabel(I18n::language() == "tr" ? "Bu işlem 2 GiB EFI + kalan kök dizin bölümünü oluşturacaktır.\nDiskteki tüm mevcut veriler SİLİNECEKTİR." : "This will create a 2 GiB EFI + remaining root partition.\nAll existing data on the disk will be DESTROYED.");
    wipeInfo->setWordWrap(true);
    wipeInfo->setObjectName("hint");
    wpLay->addWidget(wipeInfo); wpLay->addStretch();
    m_modeStack->addWidget(wipePage);

    // Page 1 — Manual (cfdisk)
    auto *manPage = new QWidget();
    auto *mpLay   = new QVBoxLayout(manPage); mpLay->setContentsMargins(0,0,0,0); mpLay->setSpacing(8);
    m_cfdiskBtn = new QPushButton(I18n::language() == "tr" ? "cfdisk Bölüm Yöneticisini Aç" : "Open cfdisk Partition Editor");
    m_cfdiskBtn->setObjectName("btnSmall");
    m_cfdiskBtn->setFixedWidth(240);
    connect(m_cfdiskBtn, &QPushButton::clicked, this, &DiskPage::launchCfdisk);
    m_partInfo = new QLabel(I18n::language() == "tr" ? "cfdisk ile diskinizi bölümlemek için yukarıdaki butona tıklayın.\nArdından EFI ve Root için kullanılacak bölümleri aşağıdan seçin." : "Click the button above to partition your disk with cfdisk.\nThen select which partitions to use as EFI and Root below.");
    m_partInfo->setObjectName("hint");
    m_partInfo->setWordWrap(true);
    mpLay->addWidget(m_cfdiskBtn);
    mpLay->addWidget(m_partInfo);
    mpLay->addStretch();
    m_modeStack->addWidget(manPage);

    // Page 2 — Replace
    auto *repPage = new QWidget();
    auto *rpLay   = new QVBoxLayout(repPage); rpLay->setContentsMargins(0,0,0,0); rpLay->setSpacing(8);
    auto *reInfo  = new QLabel("Keep your existing EFI — only format/replace the selected root partition.\nUseful for dual-boot setups.");
    reInfo->setWordWrap(true); reInfo->setObjectName("hint");
    rpLay->addWidget(reInfo);
    rpLay->addStretch();
    m_modeStack->addWidget(repPage);

    // Page 3 — Grow EFI
    auto *growPage = new QWidget();
    auto *gpLay   = new QVBoxLayout(growPage); gpLay->setContentsMargins(0,0,0,0); gpLay->setSpacing(8);
    auto *gInfo   = new QLabel("Safely resize an undersized EFI partition by borrowing space from an adjacent\npartition. Make sure no critical data is on the neighboring partition.");
    gInfo->setWordWrap(true); gInfo->setObjectName("hint");
    m_growBtn = new QPushButton("Run Grow EFI Now…");
    m_growBtn->setObjectName("btnSmall"); m_growBtn->setFixedWidth(200);
    connect(m_growBtn, &QPushButton::clicked, this, &DiskPage::runGrowEFI);
    gpLay->addWidget(gInfo); gpLay->addWidget(m_growBtn); gpLay->addStretch();
    m_modeStack->addWidget(growPage);

    lay->addWidget(m_modeStack);

    // ── Dedicated Partition Selectors (hidden in wipe mode) ────────────────
    m_partSel = new QWidget();
    auto *mpSel = new QHBoxLayout(m_partSel); mpSel->setContentsMargins(0,0,0,0); mpSel->setSpacing(10);
    auto *el = new QLabel(I18n::language() == "tr" ? "EFI Bölümü:" : "EFI Partition:");
    el->setFixedWidth(110); el->setObjectName("fieldLabel");
    m_efiBox = new QComboBox(); m_efiBox->setMinimumWidth(180);
    auto *rl2 = new QLabel(I18n::language() == "tr" ? "Kök Bölüm:" : "Root Partition:");
    rl2->setFixedWidth(110); rl2->setObjectName("fieldLabel");
    m_rootBox = new QComboBox(); m_rootBox->setMinimumWidth(180);
    mpSel->addWidget(el); mpSel->addWidget(m_efiBox);
    mpSel->addWidget(rl2); mpSel->addWidget(m_rootBox);
    mpSel->addStretch();
    lay->addWidget(m_partSel);

    // ── Filesystem + Bootloader ───────────────────────────────────────────────
    auto *br = new QHBoxLayout(); br->setSpacing(16);

    auto *fsl = new QLabel(I18n::tr("filesystem")); fsl->setFixedWidth(110); fsl->setObjectName("fieldLabel");
    m_fsBox = new QComboBox();
    m_fsBox->addItem("ext4   (Stable default)",  "ext4");
    m_fsBox->addItem("btrfs  (Snapshots + CoW)", "btrfs");
    m_fsBox->addItem("xfs    (High performance)", "xfs");
    m_fsBox->setFixedWidth(210);

    auto *bll = new QLabel(I18n::tr("bootloader")); bll->setFixedWidth(90); bll->setObjectName("fieldLabel");
    m_blBox = new QComboBox();
    m_blBox->addItem("systemd-boot  (Recommended)", "systemd-boot");
    m_blBox->addItem("GRUB", "grub");
    m_blBox->setFixedWidth(210);

    br->addWidget(fsl); br->addWidget(m_fsBox);
    br->addSpacing(14);
    br->addWidget(bll); br->addWidget(m_blBox);
    br->addStretch();
    lay->addLayout(br);
    lay->addStretch();

    connect(m_modeGroup, &QButtonGroup::idClicked, this, &DiskPage::onModeChanged);
    connect(m_diskList, &QListWidget::currentRowChanged, this, &DiskPage::refreshPartitions);
    onModeChanged(0);
}

void DiskPage::onShow() {
    m_diskList->clear();
    QProcess p;
    p.start("lsblk", {"-dno","NAME,SIZE,MODEL","-e","7,11"});
    p.waitForFinished(5000);
    const auto lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
    for (const auto &line : lines) {
        auto parts = line.trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        QString name = "/dev/" + parts[0];
        QString size = parts[1];
        QString model = parts.size() > 2 ? parts.mid(2).join(' ') : "";
        auto *item = new QListWidgetItem(QString("%1  (%2)  %3").arg(name, size, model));
        item->setData(Qt::UserRole, name);
        m_diskList->addItem(item);
    }
    if (m_diskList->count() > 0) m_diskList->setCurrentRow(0);
}

void DiskPage::onDiskChanged(int) { refreshPartitions(); }

void DiskPage::refreshPartitions() {
    auto *item = m_diskList->currentItem();
    if (!item) return;
    const QString disk = item->data(Qt::UserRole).toString();
    m_efiBox->clear(); m_rootBox->clear();

    QProcess p;
    p.start("lsblk", {"-rno","NAME,SIZE,FSTYPE", disk});
    p.waitForFinished(5000);
    const auto lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
    for (const auto &line : lines) {
        auto cols = line.trimmed().split(QRegularExpression("\\s+"));
        if (cols.isEmpty()) continue;
        const QString devName = "/dev/" + cols[0];
        if (devName == disk) continue;
        const QString sz = cols.size() > 1 ? cols[1] : "";
        const QString fs = cols.size() > 2 ? cols[2] : "";
        const QString label = QString("%1  %2  %3").arg(devName, sz, fs);
        m_efiBox->addItem(label, devName);
        m_rootBox->addItem(label, devName);
    }
}

void DiskPage::onModeChanged(int mode) {
    m_modeStack->setCurrentIndex(mode);
    m_warnLbl->setVisible(mode == 0);
    m_partSel->setVisible(mode != 0);
    refreshPartitions();
}

// Runs a command in whichever terminal the live session happens to ship.
bool DiskPage::openInTerminal(const QStringList &command) {
    const QStringList terminals = {"konsole", "kitty", "alacritty", "xterm", "x-terminal-emulator"};
    for (const QString &term : terminals) {
        QProcess p;
        p.start(term, QStringList{"-e"} + command);
        if (p.waitForStarted(2000)) { p.waitForFinished(-1); return true; }
    }
    QMessageBox::warning(this, I18n::tr("disk_title"),
        I18n::language() == "tr"
            ? "Terminal emulatörü bulunamadı, bu araç çalıştırılamadı."
            : "Could not find a terminal emulator to run this tool in.");
    return false;
}

void DiskPage::launchCfdisk() {
    auto *item = m_diskList->currentItem();
    if (!item) {
        QMessageBox::warning(this, I18n::tr("disk_title"),
            I18n::language() == "tr" ? "Önce bir disk seçin." : "Select a disk first.");
        return;
    }
    const QString disk = item->data(Qt::UserRole).toString();
    if (!openInTerminal({"cfdisk", disk})) return;
    QProcess::execute("partprobe", {disk});
    refreshPartitions();
    m_partInfo->setText(I18n::language() == "tr"
        ? "✓ cfdisk kapandı. Aşağıdan EFI ve kök bölümlerini seçin."
        : "✓ cfdisk done. Select your EFI and Root partitions below.");
}

void DiskPage::runGrowEFI() {
    auto *item = m_diskList->currentItem();
    if (!item) {
        QMessageBox::warning(this, I18n::tr("disk_title"),
            I18n::language() == "tr" ? "Önce bir disk seçin." : "Select a disk first.");
        return;
    }
    const QString disk = item->data(Qt::UserRole).toString();
    const QString script = QString(
        "echo '=== GötünOS EFI Grow Utility ==='; "
        "lsblk %1; "
        "read -p 'EFI partition (e.g. %1p1): ' EFI; "
        "read -p 'Adjacent partition to shrink (e.g. %1p2): ' SHR; "
        "read -p 'New EFI size (e.g. 2G): ' NEWSIZE; "
        "echo 'Backing up EFI to /tmp/efi_backup.tar.gz...'; "
        "mkdir -p /mnt/efi_tmp_bk; "
        "mount \"$EFI\" /mnt/efi_tmp_bk || { echo 'Could not mount the EFI partition'; exit 1; }; "
        "tar czf /tmp/efi_backup.tar.gz -C /mnt/efi_tmp_bk . ; "
        "umount /mnt/efi_tmp_bk; "
        "echo \"Now shrink $SHR and extend $EFI to $NEWSIZE with parted/gparted, then press Ctrl+D\"; "
        "bash; "
        "read -p 'Restore the EFI backup onto the resized partition? [y/N] ' yn; "
        "if [[ $yn =~ ^[Yy] ]]; then "
        "  mkfs.vfat -F32 \"$EFI\" && mount \"$EFI\" /mnt/efi_tmp_bk && "
        "  tar xzf /tmp/efi_backup.tar.gz -C /mnt/efi_tmp_bk && umount /mnt/efi_tmp_bk; "
        "fi; "
        "echo 'Done. The backup is kept at /tmp/efi_backup.tar.gz'; read -p 'Press enter to close '"
    ).arg(disk);

    if (!openInTerminal({"bash", "-c", script})) return;
    QProcess::execute("partprobe", {disk});
    refreshPartitions();
}

bool DiskPage::isValid() {
    const bool tr = (I18n::language() == "tr");
    auto *item = m_diskList->currentItem();
    if (!item) {
        QMessageBox::warning(this, I18n::tr("disk_title"),
            tr ? "Kurulum için bir disk seçin." : "Pick a disk to install to.");
        return false;
    }
    const QString disk = item->data(Qt::UserRole).toString();
    const int mode = m_modeGroup->checkedId();

    if (mode == 0) {
        // Wiping is the one thing we cannot undo, so ask before moving on.
        const auto answer = QMessageBox::warning(this, I18n::tr("disk_title"),
            (tr ? QString("%1 diskindeki TÜM veriler silinecek.\n\nDevam edilsin mi?")
                : QString("Everything on %1 will be erased.\n\nContinue?")).arg(disk),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return answer == QMessageBox::Yes;
    }

    const QString efi  = m_efiBox->currentData().toString();
    const QString root = m_rootBox->currentData().toString();
    if (efi.isEmpty() || root.isEmpty()) {
        QMessageBox::warning(this, I18n::tr("disk_title"),
            tr ? "EFI ve kök bölümlerini seçin." : "Select both an EFI and a root partition.");
        return false;
    }
    if (efi == root) {
        QMessageBox::warning(this, I18n::tr("disk_title"),
            tr ? "EFI ve kök bölümü aynı olamaz."
               : "The EFI and root partition cannot be the same one.");
        return false;
    }

    const auto answer = QMessageBox::warning(this, I18n::tr("disk_title"),
        (tr ? QString("%1 biçimlendirilecek ve üzerindeki veriler silinecek.\n\nDevam edilsin mi?")
            : QString("%1 will be formatted and everything on it will be lost.\n\nContinue?")).arg(root),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return answer == QMessageBox::Yes;
}

void DiskPage::commitToConfig() {
    auto *item = m_diskList->currentItem();
    m_cfg->targetDisk = item ? item->data(Qt::UserRole).toString() : "";

    int mode = m_modeGroup->checkedId();
    m_cfg->partMode = (mode == 0) ? "wipe" : (mode == 1) ? "manual" :
                       (mode == 2) ? "replace" : "grow";

    if (mode != 0) {
        m_cfg->efiPart  = m_efiBox->currentData().toString();
        m_cfg->rootPart = m_rootBox->currentData().toString();
    }

    m_cfg->filesystem = m_fsBox->currentData().toString();
    m_cfg->bootloader = m_blBox->currentData().toString();
}

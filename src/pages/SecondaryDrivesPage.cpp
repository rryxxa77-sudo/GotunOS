#include "SecondaryDrivesPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QProcess>
#include <QFrame>
#include <QRegularExpression>
#include <QFileInfo>

SecondaryDrivesPage::SecondaryDrivesPage(InstallConfig *cfg, QWidget *parent)
    : BasePage(cfg, parent) {

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 36, 52, 16);
    lay->setSpacing(12);

    auto *tt = new QLabel("Secondary Drives"); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(
        "Mount additional drives and optionally create symlinks (e.g. ~/Games → /mnt/games).\n"
        "All mounted drives will have Steam-compatible permissions (no 'Compatibility tool failed')."
    );
    ts->setObjectName("pageSubtitle"); ts->setWordWrap(true);
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(4);

    // Available partitions list
    auto *al = new QLabel("AVAILABLE PARTITIONS (excluding install target)");
    al->setObjectName("sectionHeader");
    lay->addWidget(al);

    m_diskList = new QListWidget();
    m_diskList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_diskList->setFixedHeight(120);
    m_diskList->setToolTip("Select one or more partitions to mount. Multi-select with Ctrl+Click.");
    lay->addWidget(m_diskList);

    auto *btnRefresh = new QPushButton("⟳ Refresh");
    btnRefresh->setObjectName("btnSmall"); btnRefresh->setFixedWidth(100);
    connect(btnRefresh, &QPushButton::clicked, this, &SecondaryDrivesPage::refreshDrives);

    auto *btnApply = new QPushButton("Add Selected →");
    btnApply->setObjectName("btnSmall"); btnApply->setFixedWidth(140);
    connect(btnApply, &QPushButton::clicked, this, [this]() {
        for (auto *item : m_diskList->selectedItems()) {
            const QString part  = item->data(Qt::UserRole).toString();
            const QString size  = item->data(Qt::UserRole+1).toString();
            const QString fs    = item->data(Qt::UserRole+2).toString();
            const QString label = item->data(Qt::UserRole+3).toString();
            bool dup = false;
            for (const auto &r : m_rows) if (r.partition == part) { dup = true; break; }
            if (!dup) addDriveRow(part, size, fs, label);
        }
        m_diskList->clearSelection();
    });

    auto *br2 = new QHBoxLayout(); br2->setSpacing(8);
    br2->addWidget(btnRefresh); br2->addWidget(btnApply); br2->addStretch();
    lay->addLayout(br2);

    // Rows area
    auto *cl = new QLabel("CONFIGURED MOUNTS");
    cl->setObjectName("sectionHeader");
    lay->addWidget(cl);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    m_rowsWidget = new QWidget();
    m_rowsLayout = new QVBoxLayout(m_rowsWidget);
    m_rowsLayout->setContentsMargins(0,0,0,0);
    m_rowsLayout->setSpacing(8);
    m_rowsLayout->addStretch();
    scroll->setWidget(m_rowsWidget);
    lay->addWidget(scroll, 1);

    auto *hint = new QLabel(
        "ℹ  Mount point must be absolute (e.g. /mnt/games).  "
        "Symlink name is relative to each user's home (e.g. Games → ~/Games).\n"
        "   NTFS/exFAT/FAT32 drives get uid=1000 permissions. "
        "Linux filesystems get chmod 777 + chown 1000:1000 for full Steam access."
    );
    hint->setWordWrap(true); hint->setObjectName("hint");
    lay->addWidget(hint);
}

void SecondaryDrivesPage::onShow() {
    m_targetDisk = m_cfg->targetDisk;
    refreshDrives();
}

void SecondaryDrivesPage::refreshDrives() {
    m_diskList->clear();

    QProcess p;
    p.start("lsblk", {"-rno","NAME,SIZE,FSTYPE,LABEL,TYPE"});
    p.waitForFinished(5000);
    const auto lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);

    for (const auto &line : lines) {
        auto cols = line.trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (cols.size() < 5) continue;
        const QString name  = "/dev/" + cols[0];
        const QString size  = cols[1];
        const QString fs    = cols[2];
        const QString label = cols.size() > 3 ? cols[3] : "";
        const QString type  = cols.last();

        if (type != "part") continue;
        // Partitions of the install target only: /dev/sda1 belongs to /dev/sda,
        // /dev/sdaa1 does not.
        static const QRegularExpression partSuffix("^p?[0-9]+$");
        if (name.startsWith(m_targetDisk)
            && partSuffix.match(name.mid(m_targetDisk.size())).hasMatch()) continue;
        if (fs.isEmpty() || fs == "swap") continue;

        auto *item = new QListWidgetItem(
            QString("  %1   %2   %3%4")
                .arg(name, size, fs, label.isEmpty() ? "" : "  [" + label + "]")
        );
        item->setData(Qt::UserRole,   name);
        item->setData(Qt::UserRole+1, size);
        item->setData(Qt::UserRole+2, fs);
        item->setData(Qt::UserRole+3, label);
        m_diskList->addItem(item);
    }

    if (m_diskList->count() == 0)
        m_diskList->addItem("  No additional formatted partitions found.");
}

void SecondaryDrivesPage::addDriveRow(const QString &partition, const QString &size,
                                       const QString &fsType, const QString &label) {
    DriveRowWidget row;
    row.partition = partition;
    row.fsType    = fsType;

    auto *card = new QFrame(); card->setObjectName("card");
    auto *cl2  = new QVBoxLayout(card); cl2->setContentsMargins(16,12,16,12); cl2->setSpacing(8);

    // Header row
    auto *header = new QHBoxLayout(); header->setSpacing(10);
    auto *icon = new QLabel("💾");
    auto *dev  = new QLabel(partition + "  " + size + "  " + fsType +
                             (label.isEmpty() ? "" : "  [" + label + "]"));
    dev->setStyleSheet("font-weight:bold; color:#1d1d1f;");
    header->addWidget(icon); header->addWidget(dev, 1);

    // Remove button
    auto *btnRm = new QPushButton("X");
    btnRm->setObjectName("btnDanger");
    btnRm->setFixedSize(28, 28);
    connect(btnRm, &QPushButton::clicked, this, [this, card, partition]() {
        card->deleteLater();
        m_rows.removeIf([&](const DriveRowWidget &r){ return r.partition == partition; });
    });
    header->addWidget(btnRm);
    cl2->addLayout(header);

    // Mount point
    auto *mrow = new QHBoxLayout(); mrow->setSpacing(10);
    auto *ml = new QLabel("Mount at:"); ml->setFixedWidth(90); ml->setObjectName("fieldLabel");
    row.mountEdit = new QLineEdit();
    QString defMnt = "/mnt/" + (label.isEmpty() ? QFileInfo(partition).fileName() : label.toLower());
    row.mountEdit->setText(defMnt);
    row.mountEdit->setPlaceholderText("/mnt/games");
    mrow->addWidget(ml); mrow->addWidget(row.mountEdit, 1);
    cl2->addLayout(mrow);

    // Symlink
    auto *srow = new QHBoxLayout(); srow->setSpacing(10);
    row.symlinkCheck = new QCheckBox("Symlink into ~/");
    row.symlinkCheck->setChecked(true);
    row.symlinkEdit  = new QLineEdit();
    row.symlinkEdit->setPlaceholderText("Games");
    row.symlinkEdit->setText(label.isEmpty() ? "Games" : label);
    row.symlinkEdit->setFixedWidth(200);
    srow->addWidget(row.symlinkCheck);
    srow->addWidget(row.symlinkEdit);
    srow->addStretch();
    cl2->addLayout(srow);

    connect(row.symlinkCheck, &QCheckBox::toggled, row.symlinkEdit, &QLineEdit::setEnabled);

    row.container = card;
    m_rows.append(row);

    int insertAt = m_rowsLayout->count() - 1;
    m_rowsLayout->insertWidget(insertAt, card);
}

void SecondaryDrivesPage::commitToConfig() {
    m_cfg->secondaryDrives.clear();
    for (const auto &r : m_rows) {
        SecondaryDrive drv;
        drv.partition   = r.partition;
        drv.fsType      = r.fsType;
        drv.mountPoint  = r.mountEdit->text().trimmed();
        drv.symlinkName = (r.symlinkCheck->isChecked()) ? r.symlinkEdit->text().trimmed() : "";
        if (!drv.partition.isEmpty() && !drv.mountPoint.isEmpty())
            m_cfg->secondaryDrives.append(drv);
    }
}

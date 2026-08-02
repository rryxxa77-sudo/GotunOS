#include "NetworkPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QThread>

NetworkPage::NetworkPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(14);

    auto *tt = new QLabel(I18n::tr("network_title"));    tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("network_subtitle")); ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(8);

    // Status + Scan button
    auto *sr = new QHBoxLayout(); sr->setSpacing(12);
    m_statusLbl = new QLabel("● Checking...");
    m_statusLbl->setStyleSheet("font-weight:bold; color:#1d1d1f;");
    m_btnScan = new QPushButton(I18n::tr("scan_networks"));
    m_btnScan->setObjectName("btnSmall");
    m_btnScan->setFixedWidth(150);
    connect(m_btnScan, &QPushButton::clicked, this, &NetworkPage::onScan);
    sr->addWidget(m_statusLbl, 1); sr->addWidget(m_btnScan);
    lay->addLayout(sr);

    // Network list
    m_netList = new QListWidget();
    m_netList->setFixedHeight(170);
    m_netList->setToolTip("Select a Wi-Fi network to connect, or click again to deselect");
    lay->addWidget(m_netList);

    // Safe declick / toggle selection using QPointer m_lastItem
    connect(m_netList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (m_lastItem == item) {
            m_netList->clearSelection();
            m_netList->setCurrentItem(nullptr);
            m_lastItem = nullptr;
        } else {
            m_lastItem = item;
        }
    });

    // Password + connect + deselect
    auto *cr = new QHBoxLayout(); cr->setSpacing(10);
    auto *pl = new QLabel(I18n::tr("wifi_password"));
    pl->setObjectName("fieldLabel");
    pl->setMinimumWidth(120);
    m_passEdit = new QLineEdit(); m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setPlaceholderText("Leave blank for open networks");
    
    m_btnConnect = new QPushButton(I18n::tr("connect"));
    m_btnConnect->setObjectName("btnSmall");
    m_btnConnect->setFixedWidth(100);
    connect(m_btnConnect, &QPushButton::clicked, this, &NetworkPage::onConnect);

    auto *btnDeselect = new QPushButton("Clear Selection");
    btnDeselect->setObjectName("btnSmall");
    btnDeselect->setFixedWidth(120);
    connect(btnDeselect, &QPushButton::clicked, this, [this]() {
        m_netList->clearSelection();
        m_netList->setCurrentItem(nullptr);
        m_lastItem = nullptr;
    });

    cr->addWidget(pl); cr->addWidget(m_passEdit, 1); cr->addWidget(m_btnConnect); cr->addWidget(btnDeselect);
    lay->addLayout(cr);

    m_reflLbl = new QLabel("");
    m_reflLbl->setObjectName("hint");
    lay->addWidget(m_reflLbl);

    lay->addSpacing(10);

    // DNS selector
    auto *dr = new QHBoxLayout(); dr->setSpacing(14);
    auto *dl = new QLabel(I18n::tr("dns_provider"));
    dl->setObjectName("fieldLabel");
    dl->setMinimumWidth(170);
    m_dnsBox = new QComboBox();
    m_dnsBox->addItem("Cloudflare  (1.1.1.1 / 1.0.0.1)", "cloudflare");
    m_dnsBox->addItem("AdGuard     (94.140.14.14)", "adguard");
    m_dnsBox->addItem("Quad9       (9.9.9.9 — Security)", "quad9");
    m_dnsBox->addItem("None  (ISP default)", "none");
    dr->addWidget(dl); dr->addWidget(m_dnsBox, 1);
    lay->addLayout(dr);

    lay->addStretch();
}

void NetworkPage::onShow() {
    QTimer::singleShot(80,  this, &NetworkPage::checkStatus);
    QTimer::singleShot(150, this, &NetworkPage::onScan);
}

void NetworkPage::checkStatus() {
    QProcess p;
    p.start("ping", {"-c","1","-W","2","1.1.1.1"});
    p.waitForFinished(4000);
    if (p.exitCode() == 0) {
        m_statusLbl->setText(I18n::language() == "tr" ? "● İnternete Bağlı" : "● Connected to Internet");
        m_statusLbl->setObjectName("statusOk");
    } else {
        m_statusLbl->setText(I18n::language() == "tr" ? "○ İnternet Yok — Aşağıdan bir ağ seçin" : "○ No Internet — Select a network below");
        m_statusLbl->setObjectName("statusFail");
    }
    m_statusLbl->style()->unpolish(m_statusLbl);
    m_statusLbl->style()->polish(m_statusLbl);
}

void NetworkPage::onScan() {
    m_lastItem = nullptr;
    m_netList->clear();
    QProcess p;
    p.start("nmcli", {"-t","-f","IN-USE,SSID,SIGNAL,SECURITY","device","wifi","list"});
    p.waitForFinished(8000);
    const auto lines = QString::fromUtf8(p.readAllStandardOutput())
                           .split('\n', Qt::SkipEmptyParts);
    for (const auto &line : lines) {
        auto parts = line.split(':');
        if (parts.size() < 4 || parts[1].isEmpty()) continue;
        bool inUse  = (parts[0] == "*");
        QString ssid = parts[1];
        QString sig  = parts[2];
        QString sec  = parts[3];
        QString txt  = QString("%1%2  [%3%]  %4")
                           .arg(inUse ? "★ " : "  ", ssid, sig, sec);
        auto *item = new QListWidgetItem(txt, m_netList);
        item->setData(Qt::UserRole, ssid);
        if (inUse) item->setForeground(QColor("#34c759"));
    }
    if (m_netList->count() == 0)
        m_netList->addItem(I18n::language() == "tr" ? "  Wi-Fi ağı bulunamadı (Kablolu ağ veya zaten bağlı)" : "  No Wi-Fi networks found (Ethernet or already connected?)");
}

void NetworkPage::onConnect() {
    auto *item = m_netList->currentItem();
    if (!item) return;
    const QString ssid = item->data(Qt::UserRole).toString();
    if (ssid.isEmpty()) return;

    m_statusLbl->setText((I18n::language() == "tr" ? "◌ Bağlanılıyor: " : "◌ Connecting to ") + ssid + "...");
    m_statusLbl->setStyleSheet("color:#ff9500; font-weight:bold;");

    QProcess p;
    const QString pass = m_passEdit->text();
    if (pass.isEmpty())
        p.start("nmcli", {"device","wifi","connect", ssid});
    else
        p.start("nmcli", {"device","wifi","connect", ssid, "password", pass});
    p.waitForFinished(20000);

    checkStatus();

    if (m_statusLbl->text().contains("Connected") || m_statusLbl->text().contains("Bağlı")) {
        m_reflLbl->setText(I18n::language() == "tr" ? "⟳ reflector ile pacman yansımaları optimize ediliyor..." : "⟳ Optimizing pacman mirrors with reflector (background)...");
        QProcess::startDetached("reflector",
            {"--latest","20","--protocol","https","--sort","rate",
             "--save","/etc/pacman.d/mirrorlist"});
    }
}

void NetworkPage::commitToConfig() {
    m_cfg->dnsProvider = m_dnsBox->currentData().toString();
}

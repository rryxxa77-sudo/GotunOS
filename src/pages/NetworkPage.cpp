#include "NetworkPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QStyle>

// nmcli -t escapes colons inside values as "\:", so a plain split() mangles
// SSIDs like "Foo:Bar".
static QStringList splitNmcli(const QString &line) {
    QStringList fields;
    QString cur;
    for (int i = 0; i < line.size(); ++i) {
        const QChar c = line.at(i);
        if (c == '\\' && i + 1 < line.size()) { cur += line.at(++i); continue; }
        if (c == ':') { fields << cur; cur.clear(); continue; }
        cur += c;
    }
    fields << cur;
    return fields;
}

NetworkPage::NetworkPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    const bool tr = (I18n::language() == "tr");

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(14);

    auto *tt = new QLabel(I18n::tr("network_title"));    tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("network_subtitle")); ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(8);

    // Status + Scan button
    auto *sr = new QHBoxLayout(); sr->setSpacing(12);
    m_statusLbl = new QLabel(tr ? "● Denetleniyor..." : "● Checking...");
    m_statusLbl->setStyleSheet("font-weight:bold; color:#1d1d1f;");
    m_btnScan = new QPushButton(I18n::tr("scan_networks"));
    m_btnScan->setObjectName("btnSmall");
    m_btnScan->setFixedWidth(150);
    connect(m_btnScan, &QPushButton::clicked, this, &NetworkPage::onScan);

    m_btnDisconnect = new QPushButton(tr ? "Bağlantıyı Kes" : "Disconnect");
    m_btnDisconnect->setObjectName("btnSmall");
    m_btnDisconnect->setFixedWidth(130);
    connect(m_btnDisconnect, &QPushButton::clicked, this, &NetworkPage::onDisconnect);

    sr->addWidget(m_statusLbl, 1); sr->addWidget(m_btnDisconnect); sr->addWidget(m_btnScan);
    lay->addLayout(sr);

    // Wired status — the TUI connected Ethernet automatically, here we at least
    // report it and let NetworkManager bring it up.
    m_wiredLbl = new QLabel();
    m_wiredLbl->setObjectName("hint");
    lay->addWidget(m_wiredLbl);

    // Network list
    m_netList = new QListWidget();
    m_netList->setFixedHeight(170);
    m_netList->setToolTip(tr ? "Bağlanmak için bir Wi-Fi ağı seçin, seçimi kaldırmak için tekrar tıklayın"
                             : "Select a Wi-Fi network to connect, or click again to deselect");
    lay->addWidget(m_netList);

    connect(m_netList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (m_lastItem == item) {
            m_netList->clearSelection();
            m_netList->setCurrentItem(nullptr);
            m_lastItem = nullptr;
        } else {
            m_lastItem = item;
        }
    });

    // Password + connect
    auto *cr = new QHBoxLayout(); cr->setSpacing(10);
    auto *pl = new QLabel(I18n::tr("wifi_password"));
    pl->setObjectName("fieldLabel");
    pl->setMinimumWidth(120);
    m_passEdit = new QLineEdit(); m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setPlaceholderText(tr ? "Açık ağlar için boş bırakın" : "Leave blank for open networks");

    m_btnConnect = new QPushButton(I18n::tr("connect"));
    m_btnConnect->setObjectName("btnSmall");
    m_btnConnect->setFixedWidth(100);
    connect(m_btnConnect, &QPushButton::clicked, this, &NetworkPage::onConnect);
    connect(m_passEdit, &QLineEdit::returnPressed, this, &NetworkPage::onConnect);

    auto *btnDeselect = new QPushButton(tr ? "Seçimi Temizle" : "Clear Selection");
    btnDeselect->setObjectName("btnSmall");
    btnDeselect->setFixedWidth(130);
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

    auto *skipHint = new QLabel(tr
        ? "Ağ isteğe bağlıdır — çevrimdışı devam edebilirsiniz, ancak paket kurulumu için internet gerekir."
        : "Network is optional here — you can continue offline, but the install itself needs internet.");
    skipHint->setWordWrap(true);
    skipHint->setObjectName("hint");
    lay->addWidget(skipHint);

    lay->addSpacing(6);

    // DNS selector
    auto *dr = new QHBoxLayout(); dr->setSpacing(14);
    auto *dl = new QLabel(I18n::tr("dns_provider"));
    dl->setObjectName("fieldLabel");
    dl->setMinimumWidth(170);
    m_dnsBox = new QComboBox();
    m_dnsBox->addItem("Cloudflare  (1.1.1.1 / 1.0.0.1)", "cloudflare");
    m_dnsBox->addItem("AdGuard     (94.140.14.14)", "adguard");
    m_dnsBox->addItem("Quad9       (9.9.9.9 — Security)", "quad9");
    m_dnsBox->addItem(tr ? "Özel DNS (kendi sunucunuz)" : "Custom DNS (your own servers)", "custom");
    m_dnsBox->addItem(tr ? "Yok (sağlayıcı varsayılanı)" : "None  (ISP default)", "none");
    dr->addWidget(dl); dr->addWidget(m_dnsBox, 1);
    lay->addLayout(dr);

    // Custom DNS fields
    m_customDns = new QWidget();
    auto *cdr = new QHBoxLayout(m_customDns);
    cdr->setContentsMargins(0, 0, 0, 0);
    cdr->setSpacing(10);
    m_dns1Edit = new QLineEdit();     m_dns1Edit->setPlaceholderText(tr ? "Birincil IP" : "Primary IP");
    m_dns2Edit = new QLineEdit();     m_dns2Edit->setPlaceholderText(tr ? "İkincil IP" : "Secondary IP");
    m_dnsHostEdit = new QLineEdit();  m_dnsHostEdit->setPlaceholderText(tr ? "TLS ana bilgisayar adı (isteğe bağlı)"
                                                                          : "TLS hostname (optional)");
    auto *cdl = new QLabel(""); cdl->setMinimumWidth(170);
    cdr->addWidget(cdl);
    cdr->addWidget(m_dns1Edit, 1); cdr->addWidget(m_dns2Edit, 1); cdr->addWidget(m_dnsHostEdit, 2);
    m_customDns->setVisible(false);
    lay->addWidget(m_customDns);

    connect(m_dnsBox, &QComboBox::currentIndexChanged, this, [this]() {
        m_customDns->setVisible(m_dnsBox->currentData().toString() == "custom");
    });

    lay->addStretch();
}

void NetworkPage::onShow() {
    // NetworkManager is not always up yet on a freshly booted live session.
    QProcess::execute("systemctl", {"start", "NetworkManager"});
    QTimer::singleShot(80,  this, &NetworkPage::checkStatus);
    QTimer::singleShot(150, this, &NetworkPage::onScan);
}

bool NetworkPage::isOnline() {
    QProcess p;
    p.start("ping", {"-c", "1", "-W", "2", "1.1.1.1"});
    p.waitForFinished(4000);
    return p.exitCode() == 0;
}

void NetworkPage::refreshDevices() {
    QProcess p;
    p.start("nmcli", {"-t", "-f", "DEVICE,TYPE,STATE", "device", "status"});
    p.waitForFinished(6000);
    const bool tr = (I18n::language() == "tr");
    QString wired;
    for (const auto &line : QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts)) {
        const auto f = splitNmcli(line);
        if (f.size() < 3 || f[1] != "ethernet") continue;
        wired = QString("%1 — %2").arg(f[0], f[2]);
        if (f[2] == "disconnected")
            QProcess::execute("nmcli", {"device", "connect", f[0]});
    }
    m_wiredLbl->setText(wired.isEmpty()
        ? (tr ? "Kablolu bağlantı bulunamadı." : "No wired connection detected.")
        : (tr ? "Kablolu: " : "Wired: ") + wired);
}

void NetworkPage::checkStatus() {
    refreshDevices();
    const bool online = isOnline();
    m_statusLbl->setText(online
        ? (I18n::language() == "tr" ? "● İnternete Bağlı" : "● Connected to Internet")
        : (I18n::language() == "tr" ? "○ İnternet Yok — Aşağıdan bir ağ seçin"
                                    : "○ No Internet — Select a network below"));
    m_statusLbl->setStyleSheet(online ? "font-weight:bold; color:#34c759;"
                                      : "font-weight:bold; color:#ff9500;");
    m_btnDisconnect->setEnabled(online);
    if (online) runReflector();
}

void NetworkPage::runReflector() {
    static bool done = false;
    if (done) return;
    done = true;
    m_reflLbl->setText(I18n::language() == "tr"
        ? "⟳ reflector ile pacman yansımaları optimize ediliyor (arka planda)..."
        : "⟳ Optimizing pacman mirrors with reflector (background)...");
    if (!QProcess::startDetached("reflector",
            {"--latest", "20", "--protocol", "https", "--sort", "rate",
             "--save", "/etc/pacman.d/mirrorlist"})) {
        m_reflLbl->setText(I18n::language() == "tr"
            ? "reflector çalıştırılamadı — varsayılan yansımalar kullanılacak."
            : "Could not run reflector — the default mirrorlist will be used.");
    }
}

void NetworkPage::onScan() {
    m_lastItem = nullptr;
    m_netList->clear();
    QProcess rescan;
    rescan.start("nmcli", {"device", "wifi", "rescan"});
    rescan.waitForFinished(8000);

    QProcess p;
    p.start("nmcli", {"-t", "-f", "IN-USE,SSID,SIGNAL,SECURITY", "device", "wifi", "list"});
    p.waitForFinished(8000);
    const auto lines = QString::fromUtf8(p.readAllStandardOutput())
                           .split('\n', Qt::SkipEmptyParts);
    for (const auto &line : lines) {
        const auto parts = splitNmcli(line);
        if (parts.size() < 4 || parts[1].isEmpty()) continue;
        const bool inUse   = (parts[0] == "*");
        const QString ssid = parts[1];
        const QString txt  = QString("%1%2  [%3%]  %4")
                                 .arg(inUse ? "★ " : "  ", ssid, parts[2], parts[3]);
        auto *item = new QListWidgetItem(txt, m_netList);
        item->setData(Qt::UserRole, ssid);
        if (inUse) item->setForeground(QColor("#34c759"));
    }
    if (m_netList->count() == 0)
        m_netList->addItem(I18n::language() == "tr"
            ? "  Wi-Fi ağı bulunamadı (Kablolu ağ veya zaten bağlı)"
            : "  No Wi-Fi networks found (Ethernet or already connected?)");
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
        p.start("nmcli", {"device", "wifi", "connect", ssid});
    else
        p.start("nmcli", {"device", "wifi", "connect", ssid, "password", pass});
    p.waitForFinished(30000);
    const bool joined = (p.exitCode() == 0);

    if (!joined) {
        m_statusLbl->setText(I18n::language() == "tr"
            ? "✗ Bağlanılamadı — parolayı kontrol edip tekrar deneyin"
            : "✗ Could not connect — check the password and try again");
        m_statusLbl->setStyleSheet("color:#ff3b30; font-weight:bold;");
        return;
    }
    checkStatus();
    onScan();
}

void NetworkPage::onDisconnect() {
    QProcess p;
    p.start("nmcli", {"-t", "-f", "DEVICE,STATE", "device", "status"});
    p.waitForFinished(6000);
    for (const auto &line : QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts)) {
        const auto f = splitNmcli(line);
        if (f.size() >= 2 && f[1] == "connected")
            QProcess::execute("nmcli", {"device", "disconnect", f[0]});
    }
    checkStatus();
    onScan();
}

void NetworkPage::commitToConfig() {
    m_cfg->dnsProvider = m_dnsBox->currentData().toString();
    if (m_cfg->dnsProvider == "custom") {
        m_cfg->dnsIp1  = m_dns1Edit->text().trimmed();
        m_cfg->dnsIp2  = m_dns2Edit->text().trimmed();
        m_cfg->dnsHost = m_dnsHostEdit->text().trimmed();
    } else {
        m_cfg->dnsIp1.clear();
        m_cfg->dnsIp2.clear();
        m_cfg->dnsHost.clear();
    }
}

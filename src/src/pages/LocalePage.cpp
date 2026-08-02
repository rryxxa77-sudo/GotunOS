#include "LocalePage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QCompleter>

static void enableSearch(QComboBox *box, const QString &placeholder) {
    box->setEditable(true);
    box->setInsertPolicy(QComboBox::NoInsert);
    if (box->lineEdit()) {
        box->lineEdit()->setPlaceholderText(placeholder);
    }
    if (box->completer()) {
        box->completer()->setFilterMode(Qt::MatchContains);
        box->completer()->setCaseSensitivity(Qt::CaseInsensitive);
        box->completer()->setCompletionMode(QCompleter::PopupCompletion);
    }
}

LocalePage::LocalePage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(16);

    auto *tt = new QLabel(I18n::tr("locale_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("locale_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(6);

    // ── System Locale ───────────────────────────────────────
    auto *lr = new QHBoxLayout(); lr->setSpacing(14);
    auto *ll = new QLabel(I18n::tr("system_locale"));
    ll->setFixedWidth(160); ll->setObjectName("fieldLabel");
    m_localeBox = new QComboBox();
    enableSearch(m_localeBox, I18n::language() == "tr" ? "Türkçe / English (Yazarak Ara)..." : "Search Language / Locale...");
    lr->addWidget(ll); lr->addWidget(m_localeBox, 1);
    lay->addLayout(lr);

    // ── Timezone ─────────────────────────────────────
    auto *tzr = new QHBoxLayout(); tzr->setSpacing(14);
    auto *tzl = new QLabel(I18n::tr("timezone"));
    tzl->setFixedWidth(160); tzl->setObjectName("fieldLabel");

    m_continentBox = new QComboBox();
    enableSearch(m_continentBox, I18n::language() == "tr" ? "Bölge Ara..." : "Search Region...");

    m_cityBox = new QComboBox();
    enableSearch(m_cityBox, I18n::language() == "tr" ? "Şehir Ara..." : "Search City...");

    connect(m_continentBox, &QComboBox::currentTextChanged, this, &LocalePage::onContinentChanged);
    tzr->addWidget(tzl); tzr->addWidget(m_continentBox, 1); tzr->addWidget(m_cityBox, 1);
    lay->addLayout(tzr);

    // ── Keyboard Layout ──────────────────────────────
    auto *kmr = new QHBoxLayout(); kmr->setSpacing(14);
    auto *kml = new QLabel(I18n::tr("keyboard_layout"));
    kml->setFixedWidth(160); kml->setObjectName("fieldLabel");
    m_keymapBox = new QComboBox();
    enableSearch(m_keymapBox, I18n::language() == "tr" ? "Klavye Düzeni Ara (tr, us...)" : "Search Layout (us, tr...)...");

    kmr->addWidget(kml); kmr->addWidget(m_keymapBox, 1);
    lay->addLayout(kmr);

    lay->addStretch();
}

void LocalePage::onShow() {
    if (m_localeBox->count() == 0) {
        loadLocales();
        loadTimezones();
        loadKeymaps();
    }
}

void LocalePage::loadLocales() {
    m_localeBox->clear();
    QFile f("/etc/locale.gen");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith('#')) line = line.mid(1).trimmed();
            if (line.isEmpty()) continue;
            if (line.contains("UTF-8") && !line.startsWith('#'))
                m_localeBox->addItem(line, line.split(' ').first());
        }
        f.close();
    }
    if (m_localeBox->count() == 0) {
        for (const auto &l : {"en_US.UTF-8 UTF-8","tr_TR.UTF-8 UTF-8","de_DE.UTF-8 UTF-8","fr_FR.UTF-8 UTF-8"})
            m_localeBox->addItem(l, QString(l).split(' ').first());
    }
    int idx = m_localeBox->findText(I18n::language() == "tr" ? "tr_TR.UTF-8" : "en_US.UTF-8", Qt::MatchStartsWith);
    if (idx >= 0) m_localeBox->setCurrentIndex(idx);
}

void LocalePage::loadTimezones() {
    m_continentBox->clear();
    QDir tz("/usr/share/zoneinfo");
    const QStringList regions = tz.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    const QStringList validRegions = {
        "Africa","America","Antarctica","Arctic","Asia","Atlantic",
        "Australia","Europe","Indian","Pacific","US","Canada","Mexico","Brazil"
    };
    for (const auto &r : regions) {
        if (validRegions.contains(r)) m_continentBox->addItem(r);
    }
    int eidx = m_continentBox->findText("Europe");
    if (eidx >= 0) { m_continentBox->setCurrentIndex(eidx); onContinentChanged("Europe"); }
}

void LocalePage::onContinentChanged(const QString &c) {
    m_cityBox->clear();
    QDir d("/usr/share/zoneinfo/" + c);
    if (!d.exists()) return;
    const auto cities = d.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto &city : cities) m_cityBox->addItem(city);
    int iidx = m_cityBox->findText(I18n::language() == "tr" ? "Istanbul" : "London");
    if (iidx < 0) iidx = m_cityBox->findText("Istanbul");
    if (iidx >= 0) m_cityBox->setCurrentIndex(iidx);
}

void LocalePage::loadKeymaps() {
    m_keymapBox->clear();
    QProcess p;
    p.start("localectl", {"list-keymaps"});
    p.waitForFinished(8000);
    const auto maps = QString::fromUtf8(p.readAllStandardOutput())
                          .split('\n', Qt::SkipEmptyParts);
    for (const auto &m : maps) m_keymapBox->addItem(m, m);

    if (m_keymapBox->count() == 0) {
        for (const auto &m : {"us","tr","trq","trf","de","fr","es","it","uk","ru","ja","cn"})
            m_keymapBox->addItem(m, m);
    }
    int uidx = m_keymapBox->findText(I18n::language() == "tr" ? "tr" : "us");
    if (uidx >= 0) {
        m_keymapBox->setCurrentIndex(uidx);
    }
}

void LocalePage::commitToConfig() {
    m_cfg->locale   = m_localeBox->currentData().toString();
    if (m_cfg->locale.isEmpty()) m_cfg->locale = m_localeBox->currentText().split(' ').first();
    m_cfg->timezone = m_continentBox->currentText() + "/" + m_cityBox->currentText();
    m_cfg->keymap   = m_keymapBox->currentData().toString();
    if (m_cfg->keymap.isEmpty()) m_cfg->keymap = m_keymapBox->currentText();
}

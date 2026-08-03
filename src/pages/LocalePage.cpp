#include "LocalePage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>

LocalePage::LocalePage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(16);

    auto *tt = new QLabel(I18n::tr("locale_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("locale_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(6);

    const bool tr = (I18n::language() == "tr");

    // ── System locale ─────────────────────────────────────────
    auto *lr = new QHBoxLayout(); lr->setSpacing(14);
    auto *ll = new QLabel(I18n::tr("system_locale"));
    ll->setFixedWidth(160); ll->setObjectName("fieldLabel");
    m_localeBox = new SearchableComboBox();
    m_localeBox->setPlaceholderText(tr ? "Yazarak arayın (ör. tr_TR)" : "Type to search (e.g. en_US)");
    lr->addWidget(ll); lr->addWidget(m_localeBox, 1);
    lay->addLayout(lr);

    // ── Timezone ──────────────────────────────────────────────
    auto *tzr = new QHBoxLayout(); tzr->setSpacing(14);
    auto *tzl = new QLabel(I18n::tr("timezone"));
    tzl->setFixedWidth(160); tzl->setObjectName("fieldLabel");

    m_continentBox = new SearchableComboBox();
    m_continentBox->setPlaceholderText(tr ? "Bölge (ör. Europe)" : "Region (e.g. Europe)");
    m_cityBox = new SearchableComboBox();
    m_cityBox->setPlaceholderText(tr ? "Şehir (ör. Istanbul)" : "City (e.g. Istanbul)");

    connect(m_continentBox, &SearchableComboBox::selectionChanged,
            this, [this](const QString &text, const QVariant &) { onContinentChanged(text); });

    tzr->addWidget(tzl); tzr->addWidget(m_continentBox, 1); tzr->addWidget(m_cityBox, 1);
    lay->addLayout(tzr);

    // ── Keyboard layout ───────────────────────────────────────
    auto *kmr = new QHBoxLayout(); kmr->setSpacing(14);
    auto *kml = new QLabel(I18n::tr("keyboard_layout"));
    kml->setFixedWidth(160); kml->setObjectName("fieldLabel");
    m_keymapBox = new SearchableComboBox();
    m_keymapBox->setPlaceholderText(tr ? "Düzen (ör. trq, us)" : "Layout (e.g. us, trq)");
    kmr->addWidget(kml); kmr->addWidget(m_keymapBox, 1);
    lay->addLayout(kmr);

    // Apply the keymap to the live session so the test field behaves like the
    // installed system will.
    connect(m_keymapBox, &SearchableComboBox::selectionChanged,
            this, [](const QString &, const QVariant &data) {
        const QString km = data.toString();
        if (!km.isEmpty()) QProcess::startDetached("loadkeys", {km});
    });

    auto *kbr = new QHBoxLayout(); kbr->setSpacing(14);
    auto *kbl = new QLabel(I18n::tr("test_keyboard"));
    kbl->setFixedWidth(160); kbl->setObjectName("fieldLabel");
    m_kbTest = new QLineEdit();
    m_kbTest->setPlaceholderText(tr ? "Buraya yazarak klavyenizi deneyin (ıöüşçğ)"
                                    : "Type here to test your layout");
    kbr->addWidget(kbl); kbr->addWidget(m_kbTest, 1);
    lay->addLayout(kbr);

    lay->addStretch();
}

void LocalePage::onShow() {
    if (m_localeBox->entryCount() == 0) {
        loadLocales();
        loadTimezones();
        loadKeymaps();
    }
}

void LocalePage::loadLocales() {
    m_localeBox->clearEntries();
    // Only UTF-8 entries: other encodings break Qt6, Plasma and DBus.
    static const QRegularExpression localeLine(R"(^[a-z]{2,3}(_[A-Za-z0-9@.\-]+)?\.UTF-8(@\S+)?\s+UTF-8$)");
    QFile f("/etc/locale.gen");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            while (line.startsWith('#')) line = line.mid(1).trimmed();
            line = line.simplified();
            if (!localeLine.match(line).hasMatch()) continue;
            m_localeBox->addEntry(line, line.section(' ', 0, 0));
        }
        f.close();
    }
    if (m_localeBox->entryCount() == 0) {
        for (const QString &l : {QStringLiteral("en_US.UTF-8 UTF-8"), QStringLiteral("tr_TR.UTF-8 UTF-8"),
                                 QStringLiteral("de_DE.UTF-8 UTF-8"), QStringLiteral("fr_FR.UTF-8 UTF-8")})
            m_localeBox->addEntry(l, l.section(' ', 0, 0));
    }
    if (!m_localeBox->selectByData(I18n::language() == "tr" ? "tr_TR.UTF-8" : "en_US.UTF-8"))
        m_localeBox->selectFirst();
}

void LocalePage::loadTimezones() {
    m_continentBox->clearEntries();
    QDir tz("/usr/share/zoneinfo");
    const QStringList validRegions = {
        "Africa","America","Antarctica","Arctic","Asia","Atlantic",
        "Australia","Europe","Indian","Pacific"
    };
    for (const QString &r : tz.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
        if (validRegions.contains(r)) m_continentBox->addEntry(r, r);

    if (!m_continentBox->selectByData("Europe")) m_continentBox->selectFirst();
}

void LocalePage::onContinentChanged(const QString &continent) {
    m_cityBox->clearEntries();
    const QString base = "/usr/share/zoneinfo/" + continent;
    QDir d(base);
    if (!d.exists()) return;

    // Zones are files, some are symlinks, and a few regions nest one level
    // deeper (America/Argentina/Buenos_Aires, America/Indiana/…).
    QDirIterator it(base, QDir::Files | QDir::System | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    QStringList cities;
    while (it.hasNext()) {
        it.next();
        cities << d.relativeFilePath(it.filePath());
    }
    cities.sort();
    for (const QString &city : cities) m_cityBox->addEntry(city, city);

    const QString preferred = (I18n::language() == "tr") ? "Istanbul" : "London";
    if (!m_cityBox->selectByData(preferred) && !m_cityBox->selectByData("Istanbul"))
        m_cityBox->selectFirst();
}

void LocalePage::loadKeymaps() {
    m_keymapBox->clearEntries();
    QProcess p;
    p.start("localectl", {"list-keymaps"});
    p.waitForFinished(8000);
    const auto maps = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
    for (const QString &m : maps) m_keymapBox->addEntry(m.trimmed(), m.trimmed());

    if (m_keymapBox->entryCount() == 0)
        for (const QString &m : {QStringLiteral("us"), QStringLiteral("tr"), QStringLiteral("trq"),
                                 QStringLiteral("trf"), QStringLiteral("de"), QStringLiteral("fr"),
                                 QStringLiteral("es"), QStringLiteral("it"), QStringLiteral("uk"),
                                 QStringLiteral("ru")})
            m_keymapBox->addEntry(m, m);

    if (!m_keymapBox->selectByData(I18n::language() == "tr" ? "trq" : "us"))
        m_keymapBox->selectFirst();
}

bool LocalePage::isValid() {
    const bool tr = (I18n::language() == "tr");
    if (!m_localeBox->hasSelection() || !m_cityBox->hasSelection() || !m_keymapBox->hasSelection()) {
        QMessageBox::warning(this, I18n::tr("locale_title"),
            tr ? "Lütfen listeden bir dil, saat dilimi ve klavye düzeni seçin."
               : "Please pick a locale, timezone and keyboard layout from the lists.");
        return false;
    }
    return true;
}

void LocalePage::commitToConfig() {
    m_cfg->locale   = m_localeBox->selectedData().toString();
    m_cfg->timezone = m_continentBox->selectedText() + "/" + m_cityBox->selectedText();
    m_cfg->keymap   = m_keymapBox->selectedData().toString();
}

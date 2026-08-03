#include "OptionsPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QRegularExpression>

static QLabel *sectionLbl(const QString &t) {
    auto *l = new QLabel(t); l->setObjectName("sectionHeader"); return l;
}

static QLabel *fldLbl(const QString &t) {
    auto *l = new QLabel(t);
    l->setObjectName("fieldLabel");
    l->setMinimumWidth(150);
    return l;
}

OptionsPage::OptionsPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(16);

    auto *tt = new QLabel(I18n::tr("opt_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("opt_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(6);

    // ── Hostname ──────────────────────────────────────────────────────────────
    lay->addWidget(sectionLbl(I18n::language() == "tr" ? "BİLGİSAYAR ADI" : "COMPUTER NAME"));

    auto *hr = new QHBoxLayout(); hr->setSpacing(12);
    m_hostEdit = new QLineEdit(m_cfg->hostname);
    m_hostEdit->setPlaceholderText("gotunos");
    m_domEdit = new QLineEdit(m_cfg->domain);
    m_domEdit->setPlaceholderText("localdomain");
    hr->addWidget(fldLbl(I18n::tr("hostname"))); hr->addWidget(m_hostEdit, 1);
    hr->addSpacing(12);
    hr->addWidget(fldLbl(I18n::tr("domain")));   hr->addWidget(m_domEdit, 1);
    lay->addLayout(hr);

    auto *hInfo = new QLabel(I18n::language() == "tr"
        ? "Ağda görünen ad. Sadece harf, rakam ve tire kullanın."
        : "The name this machine shows on the network. Letters, digits and dashes only.");
    hInfo->setWordWrap(true); hInfo->setObjectName("hint");
    lay->addWidget(hInfo);

    auto *sep1 = new QFrame(); sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("color:#e5e5ea; margin:6px 0;");
    lay->addWidget(sep1);

    // ── ZRAM ──────────────────────────────────────────────────────────────────
    lay->addWidget(sectionLbl(I18n::language() == "tr" ? "ZRAM (SIKIŞTIRILMIŞ RAM TAKAS ALANI)" : "ZRAM (COMPRESSED RAM SWAP)"));

    auto *zr = new QHBoxLayout(); zr->setSpacing(12);
    m_zramChk = new QCheckBox(I18n::language() == "tr" ? "ZRAM'i Etkinleştir (Oyunlar için önerilir)" : "Enable ZRAM  (Recommended for gaming)");
    m_zramChk->setChecked(true);
    m_zramSpin = new QSpinBox(); m_zramSpin->setRange(0, 64);
    m_zramSpin->setValue(0); m_zramSpin->setSuffix(" GiB");
    m_zramLbl = new QLabel(I18n::language() == "tr" ? "0 = otomatik (RAM ÷ 2)" : "0 = auto (RAM ÷ 2)");
    m_zramLbl->setObjectName("hint");
    connect(m_zramChk, &QCheckBox::toggled, m_zramSpin, &QSpinBox::setEnabled);
    connect(m_zramChk, &QCheckBox::toggled, m_zramLbl,  &QLabel::setEnabled);
    zr->addWidget(m_zramChk); zr->addWidget(m_zramSpin); zr->addWidget(m_zramLbl); zr->addStretch();
    lay->addLayout(zr);

    auto *zInfo = new QLabel(I18n::language() == "tr" ? "ZRAM, zstd kullanarak RAM içinde sıkıştırılmış bir takas alanı oluşturur. SSD ömrünü uzatır." : "ZRAM creates a compressed swap device in RAM using zstd. Faster and safer than swap-on-disk. Greatly reduces I/O on SSDs.");
    zInfo->setWordWrap(true); zInfo->setObjectName("hint");
    lay->addWidget(zInfo);

    // ── Separator ──────────────────────────────────────────────────────────────
    auto *sep2 = new QFrame(); sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("color:#e5e5ea; margin:6px 0;");
    lay->addWidget(sep2);

    // ── Swap ──────────────────────────────────────────────────────────────────
    lay->addWidget(sectionLbl(I18n::language() == "tr" ? "DİSK TAKAS DOSYASI (isteğe bağlı)" : "DISK SWAP FILE  (optional)"));

    auto *sr2 = new QHBoxLayout(); sr2->setSpacing(12);
    m_swapChk = new QCheckBox(I18n::language() == "tr" ? "Takas dosyası oluştur" : "Create swap file");
    m_swapChk->setChecked(false);
    m_swapSpin = new QSpinBox(); m_swapSpin->setRange(1, 64);
    m_swapSpin->setValue(4); m_swapSpin->setSuffix(" GiB");
    m_swapSpin->setEnabled(false);
    m_swapLbl = new QLabel(I18n::language() == "tr" ? "diskte takas dosyası" : "swap file on disk");
    m_swapLbl->setObjectName("hint"); m_swapLbl->setEnabled(false);
    connect(m_swapChk, &QCheckBox::toggled, m_swapSpin, &QSpinBox::setEnabled);
    connect(m_swapChk, &QCheckBox::toggled, m_swapLbl,  &QLabel::setEnabled);
    sr2->addWidget(m_swapChk); sr2->addWidget(m_swapSpin); sr2->addWidget(m_swapLbl); sr2->addStretch();
    lay->addLayout(sr2);

    auto *swInfo = new QLabel(I18n::language() == "tr" ? "Diskte takas dosyası. ZRAM etkinse genellikle gerekmez." : "A swap file on disk. Usually not needed if ZRAM is enabled.");
    swInfo->setWordWrap(true); swInfo->setObjectName("hint");
    lay->addWidget(swInfo);

    lay->addStretch();
}

bool OptionsPage::isValid() {
    static const QRegularExpression hostRe(R"(^[a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?$)");
    if (!hostRe.match(m_hostEdit->text().trimmed()).hasMatch()) {
        QMessageBox::warning(this, I18n::tr("opt_title"),
            I18n::language() == "tr"
                ? "Bilgisayar adı harf veya rakamla başlamalı ve sadece harf, rakam ve tire içermeli."
                : "The hostname must start and end with a letter or digit and may only contain "
                  "letters, digits and dashes.");
        return false;
    }
    return true;
}

void OptionsPage::commitToConfig() {
    m_cfg->hostname     = m_hostEdit->text().trimmed();
    m_cfg->domain       = m_domEdit->text().trimmed().isEmpty() ? "localdomain"
                                                               : m_domEdit->text().trimmed();
    m_cfg->zramEnabled  = m_zramChk->isChecked();
    m_cfg->zramSizeMiB  = m_zramSpin->value() * 1024;
    m_cfg->swapEnabled  = m_swapChk->isChecked();
    m_cfg->swapSizeGiB  = m_swapSpin->value();
}

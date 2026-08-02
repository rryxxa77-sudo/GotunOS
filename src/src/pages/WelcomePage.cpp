#include "WelcomePage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include <QFrame>

static const char *TOS_EN =
    "GötünOS is provided as-is without any warranties of any kind, express or implied.\n\n"
    "By proceeding with the installation of this operating system, you acknowledge and agree:\n\n"
    "1. The installation process may FORMAT, ERASE, or MODIFY partitions on your storage devices.\n"
    "   You are solely responsible for backing up any critical data before proceeding.\n\n"
    "2. The developers of GötünOS shall not be held liable for any data loss, hardware damage,\n"
    "   system instability, or other issues resulting from the installation or usage of this software.\n\n"
    "3. You are responsible for ensuring your hardware meets compatibility requirements and that\n"
    "   you comply with any third-party licenses for software installed during this process.\n\n"
    "4. GötünOS includes third-party open-source components under their respective licenses.\n\n"
    "By clicking Continue you accept all of the above.";

static const char *TOS_TR =
    "GötünOS hiçbir açık veya zımni garanti olmaksızın olduğu gibi sunulmaktadır.\n\n"
    "Bu işletim sisteminin kurulumuna devam ederek aşağıdakileri kabul etmiş olursunuz:\n\n"
    "1. Kurulum işlemi depolama cihazlarınızdaki bölümleri BİÇİMLENDİREBİLİR, SİLEBİLİR veya DEĞİŞTİREBİLİR.\n"
    "   Devam etmeden önce kritik verilerinizi yedeklemek sizin sorumluluğunuzdadır.\n\n"
    "2. GötünOS geliştiricileri, bu yazılımın kurulumundan veya kullanımından kaynaklanan herhangi bir veri kaybı,\n"
    "   donanım hasarı veya sistem kararsızlığından sorumlu tutulamaz.\n\n"
    "3. Donanımınızın uyumluluk gereksinimlerini karşıladığından emin olmak sizin sorumluluğunuzdadır.\n\n"
    "Devam Et butonuna tıklayarak yukarıdaki tüm şartları kabul etmiş olursunuz.";

WelcomePage::WelcomePage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 44, 52, 20);
    lay->setSpacing(0);

    // Header row
    auto *hrow = new QHBoxLayout(); hrow->setSpacing(20);
    auto *hex  = new QLabel("⬡");
    hex->setStyleSheet("font-size:68px; color:#007aff;");
    auto *tvlay = new QVBoxLayout(); tvlay->setSpacing(4);
    
    auto *tt = new QLabel(I18n::tr("welcome_title"));
    tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("welcome_subtitle"));
    ts->setObjectName("pageSubtitle");
    
    tvlay->addWidget(tt); tvlay->addWidget(ts);
    hrow->addWidget(hex); hrow->addLayout(tvlay, 1);
    lay->addLayout(hrow);
    lay->addSpacing(28);

    // Language row
    auto *lrow = new QHBoxLayout(); lrow->setSpacing(14);
    auto *ll = new QLabel(I18n::tr("lang_label"));
    ll->setFixedWidth(150);
    ll->setObjectName("fieldLabel");
    m_langBox = new QComboBox();
    m_langBox->addItem("English", "en");
    m_langBox->addItem("Türkçe", "tr");
    m_langBox->setFixedWidth(200);
    lrow->addWidget(ll); lrow->addWidget(m_langBox); lrow->addStretch();
    lay->addLayout(lrow);
    lay->addSpacing(22);

    // TOS
    auto *tosLbl = new QLabel(I18n::tr("tos_header"));
    tosLbl->setStyleSheet("font-size:14px; font-weight:bold; color:#1d1d1f;");
    lay->addWidget(tosLbl);
    lay->addSpacing(6);

    auto *tosBox = new QTextEdit();
    tosBox->setReadOnly(true);
    tosBox->setPlainText(TOS_EN);
    tosBox->setFixedHeight(200);
    tosBox->setStyleSheet("background-color:#ffffff; color:#1d1d1f; font-family:sans-serif; border:1px solid #d1d1d6;");
    lay->addWidget(tosBox);
    lay->addSpacing(10);

    auto *agree = new QLabel(I18n::tr("tos_agree"));
    agree->setObjectName("hint");
    lay->addWidget(agree);
    lay->addStretch();

    connect(m_langBox, &QComboBox::currentIndexChanged, this, [this, tt, ts, ll, tosLbl, tosBox, agree]() {
        QString lang = m_langBox->currentData().toString();
        I18n::setLanguage(lang);
        tt->setText(I18n::tr("welcome_title"));
        ts->setText(I18n::tr("welcome_subtitle"));
        ll->setText(I18n::tr("lang_label"));
        tosLbl->setText(I18n::tr("tos_header"));
        tosBox->setPlainText(lang == "tr" ? TOS_TR : TOS_EN);
        agree->setText(I18n::tr("tos_agree"));
    });
}

void WelcomePage::commitToConfig() {
    m_cfg->language = m_langBox->currentData().toString();
    I18n::setLanguage(m_cfg->language);
}

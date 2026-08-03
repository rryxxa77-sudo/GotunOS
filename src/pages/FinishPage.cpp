#include "FinishPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QApplication>

FinishPage::FinishPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 60, 52, 40);
    lay->setSpacing(0);

    auto *icon = new QLabel("✓");
    icon->setStyleSheet("font-size:88px; color:#34c759;");
    icon->setAlignment(Qt::AlignHCenter);
    lay->addWidget(icon);
    lay->addSpacing(16);

    auto *tt = new QLabel(I18n::tr("finish_title"));
    tt->setStyleSheet("font-size:30px; font-weight:bold; color:#1d1d1f;");
    tt->setAlignment(Qt::AlignHCenter);
    lay->addWidget(tt);
    lay->addSpacing(10);

    auto *ts = new QLabel(I18n::tr("finish_sub"));
    ts->setWordWrap(true);
    ts->setAlignment(Qt::AlignHCenter);
    ts->setObjectName("pageSubtitle");
    lay->addWidget(ts);
    lay->addSpacing(40);

    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(20);
    btnRow->addStretch();

    auto *btnReboot = new QPushButton(I18n::tr("reboot_now"));
    btnReboot->setObjectName("btnInstall");
    btnReboot->setFixedSize(200, 46);
    connect(btnReboot, &QPushButton::clicked, this, [](){
        QProcess::execute("umount", {"-R", "/mnt"});
        QProcess::startDetached("reboot", {});
    });

    auto *btnShell = new QPushButton(I18n::tr("exit_shell"));
    btnShell->setObjectName("btnBack");
    btnShell->setFixedSize(160, 46);
    connect(btnShell, &QPushButton::clicked, this, [](){
        QApplication::quit();
    });

    btnRow->addWidget(btnShell);
    btnRow->addWidget(btnReboot);
    btnRow->addStretch();
    lay->addLayout(btnRow);
    lay->addStretch();

    auto *logHint = new QLabel("Installation log saved to /var/log/gotunos_install.log on the new system.");
    logHint->setObjectName("hint");
    logHint->setAlignment(Qt::AlignHCenter);
    lay->addWidget(logHint);
}

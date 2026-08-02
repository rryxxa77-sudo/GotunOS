#include "MainWindow.h"
#include "backend/I18n.h"
#include "pages/WelcomePage.h"
#include "pages/NetworkPage.h"
#include "pages/LocalePage.h"
#include "pages/UsersPage.h"
#include "pages/HardwarePage.h"
#include "pages/DiskPage.h"
#include "pages/SecondaryDrivesPage.h"
#include "pages/OptionsPage.h"
#include "pages/SummaryPage.h"
#include "pages/ProgressPage.h"
#include "pages/FinishPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QApplication>
#include <QScreen>
#include <QStyle>

const QStringList MainWindow::STEPS = {
    "Welcome to GötünOS",
    "Network Connection",
    "Locale & Language",
    "User Accounts",
    "Hardware & Drivers",
    "Disk Partitioning",
    "Secondary Drives",
    "System Options",
    "Installation Summary",
    "Installing System",
    "Installation Complete"
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("GötünOS Installer");
    resize(960, 660);
    setMinimumSize(880, 580);

    auto *scr = QApplication::primaryScreen();
    if (scr) {
        auto g = scr->geometry();
        move((g.width()-width())/2, (g.height()-height())/2);
    }

    auto *central = new QWidget(this);
    central->setObjectName("centralWidget");
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // 1. Ultra-clean Top Progress Bar
    m_topProgress = new QProgressBar();
    m_topProgress->setObjectName("topProgressBar");
    m_topProgress->setFixedHeight(4);
    m_topProgress->setTextVisible(false);
    m_topProgress->setRange(0, STEPS.size());

    root->addWidget(m_topProgress);

    // 2. Central Viewport Pages
    buildPages();
    root->addWidget(m_stack, 1);

    // 3. Bottom Action Bar
    auto *navBar = new QWidget();
    navBar->setObjectName("navBar");
    navBar->setFixedHeight(72);
    auto *navLayout = new QHBoxLayout(navBar);
    navLayout->setContentsMargins(40, 14, 40, 14);

    m_btnBack = new QPushButton(I18n::tr("back"));
    m_btnBack->setObjectName("btnBack");
    m_btnBack->setFixedSize(120, 42);

    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_btnNext = new QPushButton(I18n::tr("continue"));
    m_btnNext->setObjectName("btnNext");
    m_btnNext->setFixedSize(160, 42);

    navLayout->addWidget(m_btnBack);
    navLayout->addWidget(spacer);
    navLayout->addWidget(m_btnNext);

    root->addWidget(navBar);

    connect(m_btnNext, &QPushButton::clicked, this, &MainWindow::onNext);
    connect(m_btnBack, &QPushButton::clicked, this, &MainWindow::onBack);

    goTo(0);
}

void MainWindow::buildPages() {
    m_stack = new QStackedWidget();
    m_stack->setObjectName("contentArea");

    m_pages.append(new WelcomePage(&m_cfg));
    m_pages.append(new NetworkPage(&m_cfg));
    m_pages.append(new LocalePage(&m_cfg));
    m_pages.append(new UsersPage(&m_cfg));
    m_pages.append(new HardwarePage(&m_cfg));
    m_pages.append(new DiskPage(&m_cfg));
    m_pages.append(new SecondaryDrivesPage(&m_cfg));
    m_pages.append(new OptionsPage(&m_cfg));
    m_pages.append(new SummaryPage(&m_cfg));
    m_pages.append(new ProgressPage(&m_cfg));
    m_pages.append(new FinishPage(&m_cfg));

    for (auto *p : m_pages) m_stack->addWidget(p);

    auto *prog = qobject_cast<ProgressPage*>(m_pages[9]);
    if (prog) connect(prog, &ProgressPage::installFinished, this, [this](){ goTo(10); });
}

void MainWindow::goTo(int idx) {
    m_cur = idx;
    m_stack->setCurrentIndex(idx);
    m_pages[idx]->onShow();

    bool installing = (idx == 9);
    bool finished   = (idx == 10);

    m_btnBack->setVisible(idx > 0 && idx < 9);
    m_btnNext->setVisible(!installing && !finished);

    m_btnBack->setText(I18n::tr("back"));

    if (idx == 8) {
        m_btnNext->setText(I18n::tr("install_now"));
        m_btnNext->setObjectName("btnInstall");
    } else {
        m_btnNext->setText(I18n::tr("continue"));
        m_btnNext->setObjectName("btnNext");
    }
    m_btnNext->style()->unpolish(m_btnNext);
    m_btnNext->style()->polish(m_btnNext);

    updateProgressHeader();
}

void MainWindow::updateProgressHeader() {
    m_topProgress->setValue(m_cur + 1);
}

void MainWindow::onNext() {
    if (!m_pages[m_cur]->isValid()) return;
    m_pages[m_cur]->commitToConfig();
    if (m_cur == 8) {
        auto *prog = qobject_cast<ProgressPage*>(m_pages[9]);
        if (prog) prog->startInstall(m_cfg);
    }
    if (m_cur < m_pages.size()-1) goTo(m_cur+1);
}

void MainWindow::onBack() {
    if (m_cur > 0) goTo(m_cur-1);
}

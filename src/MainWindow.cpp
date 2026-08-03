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

    auto *welcome = new WelcomePage(&m_cfg);
    connect(welcome, &WelcomePage::languageChanged, this, &MainWindow::rebuildPagesForLanguage);
    m_pages.append(welcome);
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

    auto *prog = qobject_cast<ProgressPage*>(m_pages[PageProgress]);
    if (prog) connect(prog, &ProgressPage::installFinished, this, [this](){ goTo(PageFinish); });
}

// Every page builds its labels in its constructor, so switching the installer
// language means throwing the later pages away and building them again.
void MainWindow::rebuildPagesForLanguage() {
    if (m_cur != PageWelcome) return;

    while (m_pages.size() > 1) {
        BasePage *page = m_pages.takeLast();
        m_stack->removeWidget(page);
        page->deleteLater();
    }

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

    for (int i = 1; i < m_pages.size(); ++i) m_stack->addWidget(m_pages[i]);

    auto *prog = qobject_cast<ProgressPage*>(m_pages[PageProgress]);
    if (prog) connect(prog, &ProgressPage::installFinished, this, [this](){ goTo(PageFinish); });

    m_btnBack->setText(I18n::tr("back"));
    m_btnNext->setText(I18n::tr("continue"));
}

void MainWindow::goTo(int idx) {
    m_cur = idx;
    m_stack->setCurrentIndex(idx);
    m_pages[idx]->onShow();

    const bool installing = (idx == PageProgress);
    const bool finished   = (idx == PageFinish);

    m_btnBack->setVisible(idx > PageWelcome && idx < PageProgress);
    m_btnNext->setVisible(!installing && !finished);

    m_btnBack->setText(I18n::tr("back"));

    if (idx == PageSummary) {
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
    if (m_cur == PageSummary) {
        auto *prog = qobject_cast<ProgressPage*>(m_pages[PageProgress]);
        if (prog) prog->startInstall(m_cfg);
    }
    if (m_cur < m_pages.size()-1) goTo(m_cur+1);
}

void MainWindow::onBack() {
    if (m_cur > 0) goTo(m_cur-1);
}

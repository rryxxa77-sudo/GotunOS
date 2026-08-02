#include "ProgressPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>

ProgressPage::ProgressPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(16);

    // Header
    auto *hrow = new QHBoxLayout(); hrow->setSpacing(14);
    auto *icon = new QLabel("⚙");
    icon->setStyleSheet("font-size:44px; color:#007aff;");
    auto *tvl = new QVBoxLayout(); tvl->setSpacing(4);
    auto *tt  = new QLabel("Installing GötünOS"); tt->setObjectName("pageTitle");
    auto *ts  = new QLabel("Do not power off or reboot the computer during installation.");
    ts->setObjectName("pageSubtitle");
    tvl->addWidget(tt); tvl->addWidget(ts);
    hrow->addWidget(icon); hrow->addLayout(tvl, 1);
    lay->addLayout(hrow);
    lay->addSpacing(8);

    // Current step + percentage
    auto *strow = new QHBoxLayout(); strow->setSpacing(8);
    m_stepLbl = new QLabel("Preparing...");
    m_stepLbl->setStyleSheet("color:#007aff; font-size:13px; font-weight:bold;");
    m_pctLbl  = new QLabel("0%");
    m_pctLbl->setStyleSheet("color:#86868b; min-width:38px;");
    m_pctLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    strow->addWidget(m_stepLbl, 1); strow->addWidget(m_pctLbl);
    lay->addLayout(strow);

    // Progress bar
    m_bar = new QProgressBar();
    m_bar->setRange(0, 100);
    m_bar->setValue(0);
    m_bar->setFixedHeight(10);
    m_bar->setTextVisible(false);
    lay->addWidget(m_bar);

    // Log output
    auto *logLbl = new QLabel("Installation Log");
    logLbl->setObjectName("sectionHeader");
    lay->addWidget(logLbl);

    m_log = new QTextEdit();
    m_log->setReadOnly(true);
    m_log->setStyleSheet("background-color:#1c1c1e; color:#30d158; border:1px solid #d1d1d6; border-radius:8px;");
    lay->addWidget(m_log, 1);
}

void ProgressPage::startInstall(const InstallConfig &cfg) {
    m_log->clear();
    m_bar->setValue(0);
    m_stepLbl->setText("Starting installation...");

    m_worker = new InstallWorker(cfg, this);
    connect(m_worker, &InstallWorker::stepStarted,    this, &ProgressPage::onStep);
    connect(m_worker, &InstallWorker::logLine,        this, &ProgressPage::onLog);
    connect(m_worker, &InstallWorker::progressChanged,this, &ProgressPage::onProgress);
    connect(m_worker, &InstallWorker::finished,       this, &ProgressPage::onFinished);
    m_worker->start();
}

void ProgressPage::onStep(const QString &name) {
    m_stepLbl->setText(name);
    m_log->append("\n\033[0m═══ " + name + " ═══");
}

void ProgressPage::onLog(const QString &line) {
    m_log->append(line);
    m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}

void ProgressPage::onProgress(int pct) {
    m_bar->setValue(pct);
    m_pctLbl->setText(QString("%1%").arg(pct));
}

void ProgressPage::onFinished(bool ok, const QString &err) {
    if (ok) {
        m_stepLbl->setText("✓ Installation complete!");
        m_stepLbl->setStyleSheet("color:#34c759; font-size:14px; font-weight:bold;");
        m_bar->setValue(100);
        m_pctLbl->setText("100%");
        m_log->append("\n✓ Installation completed successfully.");
        emit installFinished();
    } else {
        m_stepLbl->setText("✗ Installation failed");
        m_stepLbl->setStyleSheet("color:#ff3b30; font-size:14px; font-weight:bold;");
        m_log->append("\n✗ ERROR: " + err);
        m_log->append("Please check the log above. You may rerun the installer or check /var/log/gotunos-install.log");
    }
}

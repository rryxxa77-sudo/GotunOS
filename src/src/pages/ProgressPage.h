#pragma once
#include "BasePage.h"
#include "../backend/InstallWorker.h"
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>

class ProgressPage : public BasePage {
    Q_OBJECT
public:
    explicit ProgressPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void startInstall(const InstallConfig &cfg);

signals:
    void installFinished();

private slots:
    void onStep(const QString &name);
    void onLog(const QString &line);
    void onProgress(int pct);
    void onFinished(bool ok, const QString &err);

private:
    QLabel       *m_stepLbl = nullptr;
    QProgressBar *m_bar     = nullptr;
    QTextEdit    *m_log     = nullptr;
    QLabel       *m_pctLbl  = nullptr;
    InstallWorker *m_worker = nullptr;
};

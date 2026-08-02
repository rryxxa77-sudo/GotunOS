#pragma once
#include <QThread>
#include <QString>
#include <QStringList>
#include "Config.h"

class InstallWorker : public QThread {
    Q_OBJECT
public:
    explicit InstallWorker(const InstallConfig &cfg, QObject *parent = nullptr);

signals:
    void stepStarted(const QString &name);
    void logLine(const QString &line);
    void progressChanged(int pct);
    void finished(bool ok, const QString &error);

protected:
    void run() override;

private:
    InstallConfig m_cfg;

    void  log(const QString &msg);
    bool  step(const QString &name, int pct);
    bool  sh(const QStringList &script, const QString &tag);
    bool  cmd(const QString &prog, const QStringList &args = {});

    bool stepPartition();
    bool stepFormat();
    bool stepMount();
    bool stepGenfstab();
    bool stepLocale();
    bool stepHostname();
    bool stepUsers();
    bool stepPackages();
    bool stepGPU();
    bool stepPlymouth();
    bool stepBootloader();
    bool stepServices();
    bool stepDNS();
    bool stepZRAM();
    bool stepSecondaryDrives();
    bool stepAUR();
    bool stepFlatpak();
    bool stepKDEConfig();
    bool stepFirstBoot();
    bool stepBranding();
};

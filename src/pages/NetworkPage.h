#pragma once
#include "BasePage.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QWidget>

class NetworkPage : public BasePage {
    Q_OBJECT
public:
    explicit NetworkPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;

private slots:
    void onScan();
    void onConnect();
    void onDisconnect();
    void checkStatus();

private:
    bool isOnline();
    void refreshDevices();
    void runReflector();

    QListWidget *m_netList    = nullptr;
    QLineEdit   *m_passEdit   = nullptr;
    QPushButton *m_btnConnect = nullptr;
    QPushButton *m_btnScan    = nullptr;
    QPushButton *m_btnDisconnect = nullptr;
    QLabel      *m_statusLbl  = nullptr;
    QLabel      *m_wiredLbl   = nullptr;
    QLabel      *m_reflLbl    = nullptr;
    QComboBox   *m_dnsBox     = nullptr;
    QWidget     *m_customDns  = nullptr;
    QLineEdit   *m_dns1Edit   = nullptr;
    QLineEdit   *m_dns2Edit   = nullptr;
    QLineEdit   *m_dnsHostEdit = nullptr;
    QListWidgetItem *m_lastItem = nullptr;
};

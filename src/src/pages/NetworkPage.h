#pragma once
#include "BasePage.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class NetworkPage : public BasePage {
    Q_OBJECT
public:
    explicit NetworkPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;

private slots:
    void onScan();
    void onConnect();
    void checkStatus();

private:
    QListWidget *m_netList    = nullptr;
    QLineEdit   *m_passEdit   = nullptr;
    QPushButton *m_btnConnect = nullptr;
    QPushButton *m_btnScan    = nullptr;
    QLabel      *m_statusLbl  = nullptr;
    QLabel      *m_reflLbl    = nullptr;
    QComboBox   *m_dnsBox     = nullptr;
    QListWidgetItem *m_lastItem = nullptr;
};

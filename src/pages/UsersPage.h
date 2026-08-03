#pragma once
#include "BasePage.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QLabel>

struct UserRow {
    QLabel    *label    = nullptr;
    QLineEdit *username = nullptr;
    QLineEdit *password = nullptr;
    QLineEdit *confirm  = nullptr;
    QPushButton *remove = nullptr;
};

class UsersPage : public BasePage {
    Q_OBJECT
public:
    explicit UsersPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void commitToConfig() override;
    bool isValid() override;

private slots:
    void addUserRow();
    void syncSamePassword();

private:
    void renumberRows();

private:
    QWidget     *m_rowsWidget   = nullptr;
    QVBoxLayout *m_rowsLayout   = nullptr;
    QList<UserRow> m_rows;
    QCheckBox   *m_samePassChk  = nullptr;
    QLineEdit   *m_rootPass     = nullptr;
    QLineEdit   *m_rootConfirm  = nullptr;
    QCheckBox   *m_noPassSudo   = nullptr;
};

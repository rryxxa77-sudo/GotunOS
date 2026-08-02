#pragma once
#include "BasePage.h"
#include <QComboBox>
#include <QLineEdit>

class LocalePage : public BasePage {
    Q_OBJECT
public:
    explicit LocalePage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;

private slots:
    void onContinentChanged(const QString &c);
    void onKeymapChanged(const QString &km);

private:
    void loadLocales();
    void loadTimezones();
    void loadKeymaps();

    QComboBox *m_localeBox     = nullptr;
    QComboBox *m_continentBox  = nullptr;
    QComboBox *m_cityBox       = nullptr;
    QComboBox *m_keymapBox     = nullptr;
};


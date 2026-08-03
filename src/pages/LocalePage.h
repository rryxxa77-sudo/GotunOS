#pragma once
#include "BasePage.h"
#include "../widgets/SearchableComboBox.h"

class LocalePage : public BasePage {
    Q_OBJECT
public:
    explicit LocalePage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;
    bool isValid() override;

private slots:
    void onContinentChanged(const QString &continent);

private:
    void loadLocales();
    void loadTimezones();
    void loadKeymaps();

    SearchableComboBox *m_localeBox    = nullptr;
    SearchableComboBox *m_continentBox = nullptr;
    SearchableComboBox *m_cityBox      = nullptr;
    SearchableComboBox *m_keymapBox    = nullptr;
    QLineEdit          *m_kbTest       = nullptr;
};

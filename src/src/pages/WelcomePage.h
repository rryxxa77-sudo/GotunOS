#pragma once
#include "BasePage.h"
#include <QComboBox>
class WelcomePage : public BasePage {
    Q_OBJECT
public:
    explicit WelcomePage(InstallConfig *cfg, QWidget *parent = nullptr);
    void commitToConfig() override;
private:
    QComboBox *m_langBox = nullptr;
};

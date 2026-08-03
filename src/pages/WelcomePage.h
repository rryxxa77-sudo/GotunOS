#pragma once
#include "BasePage.h"
#include <QComboBox>
class WelcomePage : public BasePage {
    Q_OBJECT
public:
    explicit WelcomePage(InstallConfig *cfg, QWidget *parent = nullptr);
    void commitToConfig() override;

signals:
    void languageChanged(const QString &lang);

private:
    QComboBox *m_langBox = nullptr;
};

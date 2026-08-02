#pragma once
#include <QWidget>
#include <QLayout>
#include "../backend/Config.h"

class BasePage : public QWidget {
    Q_OBJECT
public:
    explicit BasePage(InstallConfig *cfg, QWidget *parent = nullptr);
    virtual ~BasePage();
    virtual void commitToConfig() {}
    virtual bool isValid() { return true; }
    virtual void onShow() {}

protected:
    InstallConfig *m_cfg;
};

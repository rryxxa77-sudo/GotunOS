#pragma once
#include "BasePage.h"
class FinishPage : public BasePage {
    Q_OBJECT
public:
    explicit FinishPage(InstallConfig *cfg, QWidget *parent = nullptr);
};

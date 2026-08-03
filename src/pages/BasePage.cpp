#include "BasePage.h"
BasePage::BasePage(InstallConfig *cfg, QWidget *parent)
    : QWidget(parent), m_cfg(cfg) {}
BasePage::~BasePage() = default;

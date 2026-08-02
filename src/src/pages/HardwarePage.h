#pragma once
#include "BasePage.h"
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

class HardwarePage : public BasePage {
    Q_OBJECT
public:
    explicit HardwarePage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;

private:
    void autodetect();
    QComboBox *m_gpuBox   = nullptr;
    QComboBox *m_cpuBox   = nullptr;
    QCheckBox *m_razerBox = nullptr;
    QLabel    *m_detected = nullptr;
};

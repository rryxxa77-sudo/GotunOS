#pragma once
#include "BasePage.h"
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>

class OptionsPage : public BasePage {
    Q_OBJECT
public:
    explicit OptionsPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void commitToConfig() override;

private:
    QCheckBox  *m_zramChk   = nullptr;
    QSpinBox   *m_zramSpin  = nullptr;
    QLabel     *m_zramLbl   = nullptr;
    QCheckBox  *m_swapChk   = nullptr;
    QSpinBox   *m_swapSpin  = nullptr;
    QLabel     *m_swapLbl   = nullptr;
};

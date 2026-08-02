#pragma once
#include "BasePage.h"
#include <QTextEdit>
class SummaryPage : public BasePage {
    Q_OBJECT
public:
    explicit SummaryPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
private:
    QTextEdit *m_text = nullptr;
};

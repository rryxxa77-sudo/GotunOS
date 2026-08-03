#pragma once
#include "BasePage.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QButtonGroup>
#include <QStackedWidget>

class DiskPage : public BasePage {
    Q_OBJECT
public:
    explicit DiskPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;
    bool isValid() override;

private slots:
    void onDiskChanged(int idx);
    void onModeChanged(int mode);
    void refreshPartitions();
    void launchCfdisk();
    void runGrowEFI();

private:
    bool openInTerminal(const QStringList &command);

    QWidget        *m_partSel    = nullptr;
    QListWidget    *m_diskList   = nullptr;
    QButtonGroup   *m_modeGroup  = nullptr;
    QStackedWidget *m_modeStack  = nullptr;
    QComboBox      *m_efiBox     = nullptr;
    QComboBox      *m_rootBox    = nullptr;
    QComboBox      *m_fsBox      = nullptr;
    QComboBox      *m_blBox      = nullptr;
    QLabel         *m_warnLbl    = nullptr;
    QPushButton    *m_cfdiskBtn  = nullptr;
    QPushButton    *m_growBtn    = nullptr;
    QLabel         *m_partInfo   = nullptr;
};

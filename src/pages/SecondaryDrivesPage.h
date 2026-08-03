#pragma once
#include "BasePage.h"
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QWidget>
#include <QVBoxLayout>
#include "../backend/Config.h"

struct DriveRowWidget {
    QString   partition;
    QString   fsType;
    QLineEdit *mountEdit    = nullptr;
    QCheckBox *symlinkCheck = nullptr;
    QLineEdit *symlinkEdit  = nullptr;
    QLabel    *info         = nullptr;
    QWidget   *container    = nullptr;
};

class SecondaryDrivesPage : public BasePage {
    Q_OBJECT
public:
    explicit SecondaryDrivesPage(InstallConfig *cfg, QWidget *parent = nullptr);
    void onShow() override;
    void commitToConfig() override;

private slots:
    void refreshDrives();

private:
    void addDriveRow(const QString &partition, const QString &size, const QString &fsType, const QString &label);

    QListWidget *m_diskList   = nullptr;
    QWidget     *m_rowsWidget = nullptr;
    QVBoxLayout *m_rowsLayout = nullptr;
    QList<DriveRowWidget> m_rows;
    QString     m_targetDisk;
};

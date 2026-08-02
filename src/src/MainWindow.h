#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include "backend/Config.h"

class BasePage;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onNext();
    void onBack();

private:
    void buildPages();
    void goTo(int idx);
    void updateProgressHeader();

    QStackedWidget *m_stack       = nullptr;
    QPushButton    *m_btnBack     = nullptr;
    QPushButton    *m_btnNext     = nullptr;
    QProgressBar   *m_topProgress = nullptr;

    QVector<BasePage*> m_pages;
    int m_cur = 0;
    InstallConfig m_cfg;

    static const QStringList STEPS;
};

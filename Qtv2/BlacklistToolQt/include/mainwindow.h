#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "createblacklistwidget.h"
#include "createtestsetwidget.h"
#include "encryptiontestwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUi();
    void createTabs();
    
    QTabWidget* m_tabWidget;
    CreateBlacklistWidget* m_blacklistWidget;
    CreateTestSetWidget* m_testsetWidget;
    EncryptionTestWidget* m_encryptionWidget;
};

#endif // MAINWINDOW_H

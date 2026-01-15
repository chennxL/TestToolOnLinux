#ifndef CREATETESTSETWIDGET_H
#define CREATETESTSETWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "testsetstore.h"
#include "blackliststore.h"

class CreateTestSetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateTestSetWidget(QWidget *parent = nullptr);
    ~CreateTestSetWidget();

private slots:
    void onCreateClicked();
    void onExportClicked();
    void onQueryClicked();
    
    void onTestSetStatusChanged(TestSetStore::TestSetStatus status);
    void onTestSetSizeChanged(int inside, int outside);
    void onQueryStatusChanged(TestSetStore::QueryStatus status);
    void onQueryResultChanged(int matched, int total, double time);
    
    void onTestSetCreateSuccess();
    void onTestSetCreateFailed(const QString& error);
    void onQuerySuccess();
    void onQueryFailed(const QString& error);
    void onExportSuccess(const QString& filename);
    void onExportFailed(const QString& error);
    
    void onBlacklistStatusChanged(BlacklistStore::Status status);

private:
    void setupUi();
    void setupConnections();
    bool validateSizes();
    void updateButtonStates();
    QString getTestSetStatusColor(TestSetStore::TestSetStatus status);
    
    // UI组件
    QLineEdit* m_insideInput;
    QLineEdit* m_outsideInput;
    QPushButton* m_createButton;
    QPushButton* m_exportButton;
    QPushButton* m_queryButton;
    
    QLabel* m_statusTag;
    QLabel* m_testsetSizeLabel;
    QLabel* m_queryStatusLabel;
    QLabel* m_queryResultLabel;
    QLabel* m_queryTimeLabel;
};

#endif // CREATETESTSETWIDGET_H

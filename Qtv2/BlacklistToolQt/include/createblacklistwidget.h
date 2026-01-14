#ifndef CREATEBLACKLISTWIDGET_H
#define CREATEBLACKLISTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "blackliststore.h"

class CreateBlacklistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateBlacklistWidget(QWidget *parent = nullptr);
    ~CreateBlacklistWidget();

private slots:
    void onCreateClicked();
    void onStatusChanged(BlacklistStore::Status status);
    void onSizeChanged(int size);
    void onCreateSuccess();
    void onCreateFailed(const QString& error);

private:
    void setupUi();
    void setupConnections();
    bool validateSize();
    QString getStatusColor(BlacklistStore::Status status);
    
    // UI组件
    QLineEdit* m_sizeInput;
    QPushButton* m_createButton;
    QLabel* m_statusLabel;
    QLabel* m_statusTag;
    QLabel* m_currentSizeLabel;
    
    // 状态
    bool m_isCreating;
};

#endif // CREATEBLACKLISTWIDGET_H

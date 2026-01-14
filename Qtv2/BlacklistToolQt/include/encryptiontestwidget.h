#ifndef ENCRYPTIONTESTWIDGET_H
#define ENCRYPTIONTESTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "cryptowrapper.h"  // 🔥 新增

class EncryptionTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EncryptionTestWidget(QWidget *parent = nullptr);
    ~EncryptionTestWidget();

private:
    void setupUi();
    void setupConnections();
    bool validateIdCard();
    QString getStatusColor(const QString& status);

private slots:
    void onEncryptClicked();

private:
    QLineEdit* m_inputData;
    QPushButton* m_encryptButton;
    QLabel* m_statusTag;
    QLabel* m_originalDataLabel;
    QTextEdit* m_encryptedDataBox;

    QString m_statusText;
    bool m_isEncrypting;

    CryptoWrapper m_cryptoWrapper;  // 🔥 新增：加密工具
};

#endif // ENCRYPTIONTESTWIDGET_H

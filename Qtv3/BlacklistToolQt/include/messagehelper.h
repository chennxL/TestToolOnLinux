#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QString>
#include <QWidget>

class MessageHelper
{
public:
    enum MessageType {
        Success,
        Warning,
        Error,
        Info
    };
    
    static void showMessage(QWidget* parent, const QString& message, MessageType type = Info);
    static void showSuccess(QWidget* parent, const QString& message);
    static void showWarning(QWidget* parent, const QString& message);
    static void showError(QWidget* parent, const QString& message);
    static void showInfo(QWidget* parent, const QString& message);
    
private:
    MessageHelper() = delete;
};

#endif // MESSAGEHELPER_H

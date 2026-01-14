#include "messagehelper.h"
#include <QMessageBox>

void MessageHelper::showMessage(QWidget* parent, const QString& message, MessageType type)
{
    QMessageBox::Icon icon;
    QString title;
    
    switch (type) {
    case Success:
        icon = QMessageBox::Information;
        title = "成功";
        break;
    case Warning:
        icon = QMessageBox::Warning;
        title = "警告";
        break;
    case Error:
        icon = QMessageBox::Critical;
        title = "错误";
        break;
    case Info:
    default:
        icon = QMessageBox::Information;
        title = "提示";
        break;
    }
    
    QMessageBox msgBox(parent);
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MessageHelper::showSuccess(QWidget* parent, const QString& message)
{
    showMessage(parent, message, Success);
}

void MessageHelper::showWarning(QWidget* parent, const QString& message)
{
    showMessage(parent, message, Warning);
}

void MessageHelper::showError(QWidget* parent, const QString& message)
{
    showMessage(parent, message, Error);
}

void MessageHelper::showInfo(QWidget* parent, const QString& message)
{
    showMessage(parent, message, Info);
}

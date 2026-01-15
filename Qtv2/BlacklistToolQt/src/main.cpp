#include "mainwindow.h"
#include "networkrequest.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("黑名单查询测试工具");
    app.setOrganizationName("Anthropic");
    app.setApplicationVersion("1.0.0");
    
    // 设置默认字体
    QFont font = app.font();
    font.setFamily("Microsoft YaHei, SimHei, Arial");
    app.setFont(font);
    
    // 设置API基础URL（可以通过配置文件或环境变量设置）
    NetworkRequest::instance().setBaseUrl("http://localhost:8080/api");
    
    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    
    // 设置窗口属性
    setWindowTitle("黑名单查询测试工具");
    resize(1400, 900);
    
    // 居中显示 - 使用QApplication::primaryScreen()更兼容
    QScreen* primaryScreen = QApplication::primaryScreen();
    if (primaryScreen) {
        QRect screenGeometry = primaryScreen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
    // 🔥 新增:启动时从数据库加载黑名单信息
    BlacklistStore::instance().loadFromDatabase();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // 中心Widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 创建头部
    QWidget* header = new QWidget;
    header->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "       stop:0 #2c5282, stop:1 #3b6ba8);"
        "}"
    );
    header->setFixedHeight(100);
    
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(40, 24, 40, 24);
    
    // 图标（使用Unicode字符代替）
    QLabel* icon = new QLabel("📋");
    icon->setStyleSheet("font-size: 32px; color: white;");
    headerLayout->addWidget(icon);
    
    headerLayout->addSpacing(16);
    
    // 标题
    QLabel* title = new QLabel("黑名单查询测试工具");
    title->setStyleSheet(
        "font-size: 28px;"
        "color: white;"
        "font-weight: 600;"
        "letter-spacing: 2px;"
    );
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    mainLayout->addWidget(header);
    
    // 创建Tab页
    createTabs();
    mainLayout->addWidget(m_tabWidget, 1);
}

void MainWindow::createTabs()
{
    m_tabWidget = new QTabWidget;
    
    // 设置Tab样式
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   background: white;"
        "   border: none;"
        "   border-top: 2px solid #e4e7ed;"
        "}"
        "QTabBar::tab {"
        "   background: #fafafa;"
        "   color: #606266;"
        "   font-size: 18px;"
        "   font-weight: 500;"
        "   padding: 20px 30px;"
        "   border: none;"
        "   border-bottom: 3px solid transparent;"
        "}"
        "QTabBar::tab:selected {"
        "   color: #2c5282;"
        "   font-weight: 600;"
        "   border-bottom: 3px solid #2c5282;"
        "}"
        "QTabBar::tab:hover {"
        "   color: #2c5282;"
        "}"
    );
    
    // 创建Widget实例
    m_blacklistWidget = new CreateBlacklistWidget;
    m_testsetWidget = new CreateTestSetWidget;
    m_encryptionWidget = new EncryptionTestWidget;
    
    // 为每个Tab添加内边距
    QWidget* blacklistContainer = new QWidget;
    QVBoxLayout* blacklistLayout = new QVBoxLayout(blacklistContainer);
    blacklistLayout->setContentsMargins(40, 40, 40, 40);
    blacklistLayout->addWidget(m_blacklistWidget);
    
    QWidget* testsetContainer = new QWidget;
    QVBoxLayout* testsetLayout = new QVBoxLayout(testsetContainer);
    testsetLayout->setContentsMargins(40, 40, 40, 40);
    testsetLayout->addWidget(m_testsetWidget);
    
    QWidget* encryptionContainer = new QWidget;
    QVBoxLayout* encryptionLayout = new QVBoxLayout(encryptionContainer);
    encryptionLayout->setContentsMargins(40, 40, 40, 40);
    encryptionLayout->addWidget(m_encryptionWidget);
    
    // 添加Tab页
    m_tabWidget->addTab(blacklistContainer, "创建黑名单");
    m_tabWidget->addTab(testsetContainer, "创建测试集");
    m_tabWidget->addTab(encryptionContainer, "加密测试");
    
    // 默认选中"创建测试集" (索引1)
    m_tabWidget->setCurrentIndex(1);
}

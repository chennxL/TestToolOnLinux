#include "createblacklistwidget.h"
#include "messagehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QRegularExpressionValidator>

CreateBlacklistWidget::CreateBlacklistWidget(QWidget *parent)
    : QWidget(parent)
    , m_isCreating(false)
{
    setupUi();
    setupConnections();
    
    // 初始化显示
    onStatusChanged(BlacklistStore::instance().status());
    onSizeChanged(BlacklistStore::instance().size());
}

CreateBlacklistWidget::~CreateBlacklistWidget()
{
}

void CreateBlacklistWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(40);
    
    // ========== 输入区域 ==========
    QWidget* inputSection = new QWidget;
    QHBoxLayout* inputLayout = new QHBoxLayout(inputSection);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(20);
    
    // 标签
    QLabel* label = new QLabel("黑名单规模");
    label->setStyleSheet("font-size: 18px; font-weight: 500; color: #303133;");
    label->setMinimumWidth(120);
    inputLayout->addWidget(label);
    
    // 输入框
    m_sizeInput = new QLineEdit;
    m_sizeInput->setPlaceholderText("请输入规模数量");
    m_sizeInput->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   padding: 8px 12px;"
        "   border: 1px solid #dcdfe6;"
        "   border-radius: 4px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #409EFF;"
        "}"
    );
    m_sizeInput->setFixedWidth(400);
    // 只允许输入数字
    m_sizeInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    inputLayout->addWidget(m_sizeInput);
    
    // 单位标签
    QLabel* unitLabel = new QLabel("条");
    unitLabel->setStyleSheet("font-size: 16px; color: #606266;");
    inputLayout->addWidget(unitLabel);
    
    // 创建按钮
    m_createButton = new QPushButton("创建");
    m_createButton->setStyleSheet(
        "QPushButton {"
        "   background: #2c5282;"
        "   border: 1px solid #2c5282;"
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 10px 30px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background: #3b6ba8;"
        "   border-color: #3b6ba8;"
        "}"
        "QPushButton:pressed {"
        "   background: #254166;"
        "}"
        "QPushButton:disabled {"
        "   background: #a0cfff;"
        "   border-color: #a0cfff;"
        "}"
    );
    m_createButton->setMinimumSize(120, 40);
    m_createButton->setCursor(Qt::PointingHandCursor);
    inputLayout->addWidget(m_createButton);
    
    inputLayout->addStretch();
    mainLayout->addWidget(inputSection);
    
    // ========== 结果展示区域 ==========
    QFrame* resultSection = new QFrame;
    resultSection->setStyleSheet(
        "QFrame {"
        "   background: #f9fafb;"
        "   border: 1px solid #e4e7ed;"
        "   border-radius: 8px;"
        "}"
    );
    
    QVBoxLayout* resultLayout = new QVBoxLayout(resultSection);
    resultLayout->setContentsMargins(30, 30, 30, 30);
    resultLayout->setSpacing(24);
    
    // 结果标题栏
    QWidget* resultHeader = new QWidget;
    QHBoxLayout* headerLayout = new QHBoxLayout(resultHeader);
    headerLayout->setContentsMargins(0, 0, 0, 16);
    headerLayout->setSpacing(0);
    
    QLabel* resultTitle = new QLabel("操作提示");
    resultTitle->setStyleSheet(
        "font-size: 20px;"
        "color: #409EFF;"
        "font-weight: 600;"
        "border: none;"
        "background: transparent;"
        "padding-bottom: 16px;"
    );
    headerLayout->addWidget(resultTitle);
    
    headerLayout->addStretch();
    
    // 状态标签
    m_statusTag = new QLabel("未创建");
    m_statusTag->setStyleSheet(
        "font-size: 16px;"
        "padding: 8px 20px;"
        "border-radius: 4px;"
        "background: #909399;"
        "color: white;"
        "border-bottom: none;"
    );
    m_statusTag->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_statusTag);
    
    resultLayout->addWidget(resultHeader);
    
    // 分隔线
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background: #e4e7ed; margin: 0; padding: 0;");
    line->setFixedHeight(2);
    resultLayout->addWidget(line);
    
    // 当前规模显示
    QWidget* sizeWidget = new QWidget;
    QHBoxLayout* sizeLayout = new QHBoxLayout(sizeWidget);
    sizeLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* sizeLabel = new QLabel("当前黑名单规模：");
    sizeLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   color: #606266;"
        "   font-weight: 500;"
        "   border: none;"  // ← 添加这行
        "   background: transparent;"  // ← 添加这行
        "}"
        );
    sizeLayout->addWidget(sizeLabel);
    
    m_currentSizeLabel = new QLabel("0");
    m_currentSizeLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 20px;"
        "   color: #303133;"
        "   font-weight: 600;"
        "   border: none;"  // ← 添加这行
        "   background: transparent;"  // ← 添加这行
        "}"
        );
    sizeLayout->addWidget(m_currentSizeLabel);
    
    sizeLayout->addStretch();
    resultLayout->addWidget(sizeWidget);
    
    resultLayout->addStretch();
    mainLayout->addWidget(resultSection);
}

void CreateBlacklistWidget::setupConnections()
{
    // 按钮点击
    connect(m_createButton, &QPushButton::clicked, this, &CreateBlacklistWidget::onCreateClicked);
    
    // Store信号
    connect(&BlacklistStore::instance(), &BlacklistStore::statusChanged,
            this, &CreateBlacklistWidget::onStatusChanged);
    connect(&BlacklistStore::instance(), &BlacklistStore::sizeChanged,
            this, &CreateBlacklistWidget::onSizeChanged);
    connect(&BlacklistStore::instance(), &BlacklistStore::createSuccess,
            this, &CreateBlacklistWidget::onCreateSuccess);
    connect(&BlacklistStore::instance(), &BlacklistStore::createFailed,
            this, &CreateBlacklistWidget::onCreateFailed);
}

bool CreateBlacklistWidget::validateSize()
{
    QString text = m_sizeInput->text().trimmed();
    
    if (text.isEmpty()) {
        MessageHelper::showWarning(this, "请输入1-5000万之间的整数");
        return false;
    }
    
    bool ok;
    int size = text.toInt(&ok);
    
    if (!ok) {
        MessageHelper::showWarning(this, "请输入1-5000万之间的整数");
        return false;
    }
    
    if (size < 1 || size > 50000000) {
        MessageHelper::showWarning(this, "请输入1-5000万之间的整数");
        return false;
    }
    
    return true;
}

void CreateBlacklistWidget::onCreateClicked()
{
    if (!validateSize()) {
        return;
    }
    
    int size = m_sizeInput->text().toInt();
    BlacklistStore::instance().createBlacklist(size);
}

void CreateBlacklistWidget::onStatusChanged(BlacklistStore::Status status)
{
    QString statusText = BlacklistStore::instance().statusText();
    QString color = getStatusColor(status);
    
    m_statusTag->setText(statusText);
    m_statusTag->setStyleSheet(QString(
        "font-size: 16px;"
        "padding: 8px 20px;"
        "border-radius: 4px;"
        "background: %1;"
        "color: white;"
        "border-bottom: none;"
    ).arg(color));
    
    // 更新按钮状态
    bool isCreating = (status == BlacklistStore::Creating);
    m_createButton->setEnabled(!isCreating);
    m_createButton->setText(isCreating ? "创建中..." : "创建");
}

void CreateBlacklistWidget::onSizeChanged(int size)
{
    m_currentSizeLabel->setText(QString::number(size));
}

void CreateBlacklistWidget::onCreateSuccess()
{
    MessageHelper::showSuccess(this, "黑名单创建成功");
}

void CreateBlacklistWidget::onCreateFailed(const QString& error)
{
    MessageHelper::showError(this, error.isEmpty() ? "黑名单创建失败" : error);
}

QString CreateBlacklistWidget::getStatusColor(BlacklistStore::Status status)
{
    switch (status) {
    case BlacklistStore::Created:
        return "#67c23a"; // 成功-绿色
    case BlacklistStore::Creating:
        return "#e6a23c"; // 警告-橙色
    case BlacklistStore::CreateFailed:
        return "#f56c6c"; // 错误-红色
    case BlacklistStore::NotCreated:
    default:
        return "#909399"; // 信息-灰色
    }
}

#include "createtestsetwidget.h"
#include "messagehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QRegularExpressionValidator>

CreateTestSetWidget::CreateTestSetWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
    
    // 初始化显示
    onTestSetStatusChanged(TestSetStore::instance().testSetStatus());
    onQueryStatusChanged(TestSetStore::instance().queryStatus());
    updateButtonStates();
}

CreateTestSetWidget::~CreateTestSetWidget()
{
}

void CreateTestSetWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(40);
    
    // ========== 输入区域 ==========
    QWidget* inputSection = new QWidget;
    QVBoxLayout* inputSectionLayout = new QVBoxLayout(inputSection);
    inputSectionLayout->setContentsMargins(0, 0, 0, 0);
    inputSectionLayout->setSpacing(24);
    
    // 输入框行
    QWidget* inputRow = new QWidget;
    QHBoxLayout* inputLayout = new QHBoxLayout(inputRow);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(20);
    
    // 标签
    QLabel* label = new QLabel("测试集规模");
    label->setStyleSheet("font-size: 18px; font-weight: 500; color: #303133;");
    label->setMinimumWidth(120);
    inputLayout->addWidget(label);
    
    // 库内输入框
    QWidget* insideWidget = new QWidget;
    QHBoxLayout* insideLayout = new QHBoxLayout(insideWidget);
    insideLayout->setContentsMargins(0, 0, 0, 0);
    insideLayout->setSpacing(8);
    
    QLabel* insidePrefix = new QLabel("库内");
    insidePrefix->setStyleSheet(
        "background: #f5f7fa;"
        "color: #606266;"
        "font-weight: 500;"
        "padding: 8px 15px;"
        "border: 1px solid #dcdfe6;"
        "border-right: none;"
        "border-top-left-radius: 4px;"
        "border-bottom-left-radius: 4px;"
    );
    insideLayout->addWidget(insidePrefix);
    
    m_insideInput = new QLineEdit;
    m_insideInput->setPlaceholderText("请输入库内规模数量");
    m_insideInput->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   padding: 8px 12px;"
        "   border: 1px solid #dcdfe6;"
        "   border-left: none;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #409EFF;"
        "}"
    );
    m_insideInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    insideLayout->addWidget(m_insideInput);
    
    QLabel* insideSuffix = new QLabel("个");
    insideSuffix->setStyleSheet(
        "background: #f5f7fa;"
        "color: #606266;"
        "font-weight: 500;"
        "padding: 8px 15px;"
        "border: 1px solid #dcdfe6;"
        "border-left: none;"
        "border-top-right-radius: 4px;"
        "border-bottom-right-radius: 4px;"
    );
    insideLayout->addWidget(insideSuffix);
    insideWidget->setFixedWidth(300);
    inputLayout->addWidget(insideWidget);
    
    // 库外输入框
    QWidget* outsideWidget = new QWidget;
    QHBoxLayout* outsideLayout = new QHBoxLayout(outsideWidget);
    outsideLayout->setContentsMargins(0, 0, 0, 0);
    outsideLayout->setSpacing(8);
    
    QLabel* outsidePrefix = new QLabel("库外");
    outsidePrefix->setStyleSheet(
        "background: #f5f7fa;"
        "color: #606266;"
        "font-weight: 500;"
        "padding: 8px 15px;"
        "border: 1px solid #dcdfe6;"
        "border-right: none;"
        "border-top-left-radius: 4px;"
        "border-bottom-left-radius: 4px;"
    );
    outsideLayout->addWidget(outsidePrefix);
    
    m_outsideInput = new QLineEdit;
    m_outsideInput->setPlaceholderText("请输入库外规模数量");
    m_outsideInput->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   padding: 8px 12px;"
        "   border: 1px solid #dcdfe6;"
        "   border-left: none;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #409EFF;"
        "}"
    );
    m_outsideInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    outsideLayout->addWidget(m_outsideInput);
    
    QLabel* outsideSuffix = new QLabel("个");
    outsideSuffix->setStyleSheet(
        "background: #f5f7fa;"
        "color: #606266;"
        "font-weight: 500;"
        "padding: 8px 15px;"
        "border: 1px solid #dcdfe6;"
        "border-left: none;"
        "border-top-right-radius: 4px;"
        "border-bottom-right-radius: 4px;"
    );
    outsideLayout->addWidget(outsideSuffix);
    outsideWidget->setFixedWidth(300);
    inputLayout->addWidget(outsideWidget);
    
    inputLayout->addStretch();
    inputSectionLayout->addWidget(inputRow);
    
    // 按钮行
    QWidget* buttonRow = new QWidget;
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonRow);
    buttonLayout->setContentsMargins(140, 0, 0, 0);
    buttonLayout->setSpacing(16);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background: #2c5282;"
        "   border: 1px solid #2c5282;"
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 10px 30px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover:!disabled {"
        "   background: #3b6ba8;"
        "   border-color: #3b6ba8;"
        "}"
        "QPushButton:pressed {"
        "   background: #254166;"
        "}"
        "QPushButton:disabled {"
        "   background: #a0cfff;"
        "   border-color: #a0cfff;"
        "}";
    
    m_createButton = new QPushButton("创建");
    m_createButton->setStyleSheet(buttonStyle);
    m_createButton->setMinimumSize(140, 40);
    m_createButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_createButton);
    
    m_exportButton = new QPushButton("导出");
    m_exportButton->setStyleSheet(buttonStyle);
    m_exportButton->setMinimumSize(140, 40);
    m_exportButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_exportButton);
    
    m_queryButton = new QPushButton("黑名单查询");
    m_queryButton->setStyleSheet(
        "QPushButton {"
        "   background: #67c23a;"
        "   border: 1px solid #67c23a;"
        "   color: white;"
        "   font-size: 16px;"
        "   padding: 10px 30px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover:!disabled {"
        "   background: #85ce61;"
        "   border-color: #85ce61;"
        "}"
        "QPushButton:pressed {"
        "   background: #5daf34;"
        "}"
        "QPushButton:disabled {"
        "   background: #b3e19d;"
        "   border-color: #b3e19d;"
        "}"
    );
    m_queryButton->setMinimumSize(140, 40);
    m_queryButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_queryButton);
    
    buttonLayout->addStretch();
    inputSectionLayout->addWidget(buttonRow);
    
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
    
    QLabel* resultTitle = new QLabel("操作提示");
    resultTitle->setStyleSheet(
        "font-size: 20px;"
        "color: #409EFF;"
        "font-weight: 600;"
        "border-bottom: 2px solid #e4e7ed;"
        "   border: none;"  // ← 添加这行
        "   background: transparent;"  // ← 添加这行
        "padding-bottom: 16px;"
    );
    headerLayout->addWidget(resultTitle);
    headerLayout->addStretch();
    
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
    line->setStyleSheet("background: #e4e7ed;");
    line->setFixedHeight(2);
    resultLayout->addWidget(line);
    
    // 结果项样式
    QString labelStyle = "font-size: 18px; color: #606266; font-weight: 500; border: none; background:transparent;";
    QString valueStyle = "font-size: 18px; color: #303133; font-weight: 600; margin-left: 8px;border: none; background:transparent;";
    
    // 测试集规模
    QWidget* sizeWidget = new QWidget;
    QHBoxLayout* sizeLayout = new QHBoxLayout(sizeWidget);
    sizeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* sizeLabel = new QLabel("测试集规模：");
    sizeLabel->setStyleSheet(labelStyle);
    sizeLayout->addWidget(sizeLabel);
    m_testsetSizeLabel = new QLabel("0个库内/0个库外");
    m_testsetSizeLabel->setStyleSheet(valueStyle);
    sizeLayout->addWidget(m_testsetSizeLabel);
    sizeLayout->addStretch();
    resultLayout->addWidget(sizeWidget);
    
    // 查询状态
    QWidget* queryStatusWidget = new QWidget;
    QHBoxLayout* queryStatusLayout = new QHBoxLayout(queryStatusWidget);
    queryStatusLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* queryStatusLabelText = new QLabel("查询状态：");
    queryStatusLabelText->setStyleSheet(labelStyle);
    queryStatusLayout->addWidget(queryStatusLabelText);
    m_queryStatusLabel = new QLabel("未执行");
    m_queryStatusLabel->setStyleSheet(valueStyle);
    queryStatusLayout->addWidget(m_queryStatusLabel);
    queryStatusLayout->addStretch();
    resultLayout->addWidget(queryStatusWidget);
    
    // 查询结果
    QWidget* queryResultWidget = new QWidget;
    QHBoxLayout* queryResultLayout = new QHBoxLayout(queryResultWidget);
    queryResultLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* queryResultLabelText = new QLabel("查询结果：");
    queryResultLabelText->setStyleSheet(labelStyle);
    queryResultLayout->addWidget(queryResultLabelText);
    m_queryResultLabel = new QLabel("-");
    m_queryResultLabel->setStyleSheet(valueStyle);
    queryResultLayout->addWidget(m_queryResultLabel);
    queryResultLayout->addStretch();
    resultLayout->addWidget(queryResultWidget);
    
    // 查询耗时
    QWidget* queryTimeWidget = new QWidget;
    QHBoxLayout* queryTimeLayout = new QHBoxLayout(queryTimeWidget);
    queryTimeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* queryTimeLabelText = new QLabel("查询耗时：");
    queryTimeLabelText->setStyleSheet(labelStyle);
    queryTimeLayout->addWidget(queryTimeLabelText);
    m_queryTimeLabel = new QLabel("-");
    m_queryTimeLabel->setStyleSheet(valueStyle);
    queryTimeLayout->addWidget(m_queryTimeLabel);
    queryTimeLayout->addStretch();
    resultLayout->addWidget(queryTimeWidget);
    
    resultLayout->addStretch();
    mainLayout->addWidget(resultSection);
}

void CreateTestSetWidget::setupConnections()
{
    // 按钮点击
    connect(m_createButton, &QPushButton::clicked, this, &CreateTestSetWidget::onCreateClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &CreateTestSetWidget::onExportClicked);
    connect(m_queryButton, &QPushButton::clicked, this, &CreateTestSetWidget::onQueryClicked);
    
    // TestSetStore 信号
    connect(&TestSetStore::instance(), &TestSetStore::testSetStatusChanged,
            this, &CreateTestSetWidget::onTestSetStatusChanged);
    connect(&TestSetStore::instance(), &TestSetStore::testSetSizeChanged,
            this, &CreateTestSetWidget::onTestSetSizeChanged);
    connect(&TestSetStore::instance(), &TestSetStore::queryStatusChanged,
            this, &CreateTestSetWidget::onQueryStatusChanged);
    connect(&TestSetStore::instance(), &TestSetStore::queryResultChanged,
            this, &CreateTestSetWidget::onQueryResultChanged);
    connect(&TestSetStore::instance(), &TestSetStore::testSetCreateSuccess,
            this, &CreateTestSetWidget::onTestSetCreateSuccess);
    connect(&TestSetStore::instance(), &TestSetStore::testSetCreateFailed,
            this, &CreateTestSetWidget::onTestSetCreateFailed);
    connect(&TestSetStore::instance(), &TestSetStore::querySuccess,
            this, &CreateTestSetWidget::onQuerySuccess);
    connect(&TestSetStore::instance(), &TestSetStore::queryFailed,
            this, &CreateTestSetWidget::onQueryFailed);
    connect(&TestSetStore::instance(), &TestSetStore::exportSuccess,
            this, &CreateTestSetWidget::onExportSuccess);
    connect(&TestSetStore::instance(), &TestSetStore::exportFailed,
            this, &CreateTestSetWidget::onExportFailed);
    
    // BlacklistStore 信号
    connect(&BlacklistStore::instance(), &BlacklistStore::statusChanged,
            this, &CreateTestSetWidget::onBlacklistStatusChanged);
}

bool CreateTestSetWidget::validateSizes()
{
    QString insideText = m_insideInput->text().trimmed();
    QString outsideText = m_outsideInput->text().trimmed();
    
    int insideSize = insideText.isEmpty() ? 0 : insideText.toInt();
    int outsideSize = outsideText.isEmpty() ? 0 : outsideText.toInt();
    
    if (insideSize < 0 || insideSize > 5000 || outsideSize < 0 || outsideSize > 5000) {
        MessageHelper::showWarning(this, "请分别输入0-5000之间的整数,并保证总规模大于0");
        return false;
    }
    
    if (insideSize + outsideSize < 1) {
        MessageHelper::showWarning(this, "请分别输入0-5000之间的整数,并保证总规模大于0");
        return false;
    }
    
    return true;
}

void CreateTestSetWidget::updateButtonStates()
{
    bool blacklistCreated = (BlacklistStore::instance().status() == BlacklistStore::Created);
    bool testsetCreated = (TestSetStore::instance().testSetStatus() == TestSetStore::Created);
    bool queryCompleted = (TestSetStore::instance().queryStatus() == TestSetStore::QueryCompleted);
    bool isQuerying = (TestSetStore::instance().queryStatus() == TestSetStore::Querying);
    
    // 创建按钮：黑名单已创建
    m_createButton->setEnabled(blacklistCreated);
    
    // 导出按钮：查询已完成
    m_exportButton->setEnabled(queryCompleted);
    
    // 查询按钮：黑名单和测试集都已创建，且未在查询中
    m_queryButton->setEnabled(blacklistCreated && testsetCreated && !isQuerying);
}

void CreateTestSetWidget::onCreateClicked()
{
    if (BlacklistStore::instance().status() != BlacklistStore::Created) {
        MessageHelper::showWarning(this, "请先创建黑名单库");
        return;
    }
    
    if (!validateSizes()) {
        return;
    }
    
    int insideSize = m_insideInput->text().isEmpty() ? 0 : m_insideInput->text().toInt();
    int outsideSize = m_outsideInput->text().isEmpty() ? 0 : m_outsideInput->text().toInt();
    
    TestSetStore::instance().createTestSet(insideSize, outsideSize);
}

void CreateTestSetWidget::onExportClicked()
{
    if (TestSetStore::instance().queryStatus() != TestSetStore::QueryCompleted) {
        MessageHelper::showWarning(this, "请先完成查询");
        return;
    }
    
    TestSetStore::instance().exportResults();
}

void CreateTestSetWidget::onQueryClicked()
{
    if (BlacklistStore::instance().status() != BlacklistStore::Created) {
        MessageHelper::showWarning(this, "请先创建黑名单");
        return;
    }
    
    if (TestSetStore::instance().testSetStatus() != TestSetStore::Created) {
        MessageHelper::showWarning(this, "请先创建测试集");
        return;
    }
    
    TestSetStore::instance().queryBlacklist();
}

void CreateTestSetWidget::onTestSetStatusChanged(TestSetStore::TestSetStatus status)
{
    QString statusText = TestSetStore::instance().testSetStatusText();
    QString color = getTestSetStatusColor(status);
    
    m_statusTag->setText(statusText);
    m_statusTag->setStyleSheet(QString(
        "font-size: 16px;"
        "padding: 8px 20px;"
        "border-radius: 4px;"
        "background: %1;"
        "color: white;"
        "border-bottom: none;"
    ).arg(color));
    
    updateButtonStates();
}

void CreateTestSetWidget::onTestSetSizeChanged(int inside, int outside)
{
    if (TestSetStore::instance().testSetStatus() == TestSetStore::Created) {
        m_testsetSizeLabel->setText(QString("%1个库内/%2个库外").arg(inside).arg(outside));
    } else {
        m_testsetSizeLabel->setText("0个库内/0个库外");
    }
}

void CreateTestSetWidget::onQueryStatusChanged(TestSetStore::QueryStatus status)
{
    m_queryStatusLabel->setText(TestSetStore::instance().queryStatusText());
    
    bool isQuerying = (status == TestSetStore::Querying);
    m_queryButton->setText(isQuerying ? "查询中..." : "黑名单查询");
    
    updateButtonStates();
}

void CreateTestSetWidget::onQueryResultChanged(int matched, int total, double time)
{
    if (TestSetStore::instance().queryStatus() == TestSetStore::QueryCompleted) {
        m_queryResultLabel->setText(QString("%1条匹配/%2条测试").arg(matched).arg(total));
        m_queryTimeLabel->setText(QString("%1秒").arg(time, 0, 'f', 2));
    } else {
        m_queryResultLabel->setText("-");
        m_queryTimeLabel->setText("-");
    }
}

void CreateTestSetWidget::onTestSetCreateSuccess()
{
    MessageHelper::showSuccess(this, "测试集创建成功");
}

void CreateTestSetWidget::onTestSetCreateFailed(const QString& error)
{
    MessageHelper::showError(this, error.isEmpty() ? "测试集创建失败" : error);
}

void CreateTestSetWidget::onQuerySuccess()
{
    MessageHelper::showSuccess(this, "查询完成");
}

void CreateTestSetWidget::onQueryFailed(const QString& error)
{
    MessageHelper::showError(this, error.isEmpty() ? "查询失败" : error);
}

void CreateTestSetWidget::onExportSuccess(const QString& filename)
{
    MessageHelper::showSuccess(this, QString("查询结果导出成功\n保存路径: %1").arg(filename));
}

void CreateTestSetWidget::onExportFailed(const QString& error)
{
    MessageHelper::showError(this, error.isEmpty() ? "导出失败，请重试" : error);
}

void CreateTestSetWidget::onBlacklistStatusChanged(BlacklistStore::Status)
{
    updateButtonStates();
}

QString CreateTestSetWidget::getTestSetStatusColor(TestSetStore::TestSetStatus status)
{
    switch (status) {
    case TestSetStore::Created:
        return "#67c23a";
    case TestSetStore::Creating:
        return "#e6a23c";
    case TestSetStore::CreateFailed:
        return "#f56c6c";
    case TestSetStore::NotCreated:
    default:
        return "#909399";
    }
}

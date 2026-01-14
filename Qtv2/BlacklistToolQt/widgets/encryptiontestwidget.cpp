#include "encryptiontestwidget.h"
#include "messagehelper.h"
#include "apiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QRegularExpression>

EncryptionTestWidget::EncryptionTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_statusText("未加密")
    , m_isEncrypting(false)
    , m_cryptoWrapper(this)
{
    setupUi();
    setupConnections();
}

EncryptionTestWidget::~EncryptionTestWidget()
{
}

void EncryptionTestWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(40);
    
    // ========== 输入区域 ==========
    QWidget* inputSection = new QWidget;
    QHBoxLayout* inputLayout = new QHBoxLayout(inputSection);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(20);
    
    QLabel* label = new QLabel("输入数据");
    label->setStyleSheet("font-size: 18px; font-weight: 500; color: #303133;");
    label->setMinimumWidth(120);
    inputLayout->addWidget(label);
    
    m_inputData = new QLineEdit;
    m_inputData->setPlaceholderText("请输入要加密的身份证号");
    m_inputData->setMaxLength(18);
    m_inputData->setStyleSheet(
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
    m_inputData->setMinimumWidth(600);
    inputLayout->addWidget(m_inputData);
    
    m_encryptButton = new QPushButton("加密");
    m_encryptButton->setStyleSheet(
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
        "}"
    );
    m_encryptButton->setMinimumSize(120, 40);
    m_encryptButton->setCursor(Qt::PointingHandCursor);
    inputLayout->addWidget(m_encryptButton);
    
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
    
    QLabel* resultTitle = new QLabel("加密结果");
    resultTitle->setStyleSheet(
        "font-size: 20px;"
        "color: #409EFF;"
        "font-weight: 600;"
        "border-bottom: 2px solid #e4e7ed;"
        "padding-bottom: 16px;"
        "border: none;"
        "background:transparent;"
    );
    headerLayout->addWidget(resultTitle);
    headerLayout->addStretch();
    
    m_statusTag = new QLabel(m_statusText);
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
    
    // 原始数据
    QWidget* originalWidget = new QWidget;
    QVBoxLayout* originalLayout = new QVBoxLayout(originalWidget);
    originalLayout->setContentsMargins(0, 0, 0, 0);
    originalLayout->setSpacing(8);
    
    QLabel* originalLabel = new QLabel("原始数据：");
    originalLabel->setStyleSheet("font-size: 16px; color: #606266; font-weight: 500;border: none; background:transparent;");
    originalLayout->addWidget(originalLabel);
    
    m_originalDataLabel = new QLabel("-");
    m_originalDataLabel->setStyleSheet("font-size: 18px; color: #409EFF; font-weight: 600;border: none; background:transparent;");
    originalLayout->addWidget(m_originalDataLabel);
    
    resultLayout->addWidget(originalWidget);
    
    // 加密数据
    QWidget* encryptedWidget = new QWidget;
    QVBoxLayout* encryptedLayout = new QVBoxLayout(encryptedWidget);
    encryptedLayout->setContentsMargins(0, 0, 0, 0);
    encryptedLayout->setSpacing(8);
    
    QLabel* encryptedLabel = new QLabel("加密数据：");
    encryptedLabel->setStyleSheet("font-size: 16px; color: #606266; font-weight: 500;border: none; background:transparent;");
    encryptedLayout->addWidget(encryptedLabel);
    
    m_encryptedDataBox = new QTextEdit;
    m_encryptedDataBox->setReadOnly(true);
    m_encryptedDataBox->setText("-");
    m_encryptedDataBox->setStyleSheet(
        "QTextEdit {"
        "   background: white;"
        "   border: 1px solid #dcdfe6;"
        "   border-radius: 4px;"
        "   padding: 16px;"
        "   font-family: 'Courier New', monospace;"
        "   font-size: 14px;"
        "   color: #606266;"
        "   line-height: 1.6;"
        "}"
    );
    m_encryptedDataBox->setMinimumHeight(80);
    encryptedLayout->addWidget(m_encryptedDataBox);
    
    resultLayout->addWidget(encryptedWidget);
    resultLayout->addStretch();
    mainLayout->addWidget(resultSection);
}

void EncryptionTestWidget::setupConnections()
{
    connect(m_encryptButton, &QPushButton::clicked, this, &EncryptionTestWidget::onEncryptClicked);
}

bool EncryptionTestWidget::validateIdCard()
{
    QString idCard = m_inputData->text().trimmed();
    
    if (idCard.isEmpty()) {
        MessageHelper::showWarning(this, "请输入要加密的身份证号");
        return false;
    }
    
    // 简单验证身份证号格式
    QRegularExpression re("^[1-9]\\d{5}(18|19|20)\\d{2}(0[1-9]|1[0-2])(0[1-9]|[12]\\d|3[01])\\d{3}[0-9Xx]$");
    if (!re.match(idCard).hasMatch()) {
        MessageHelper::showWarning(this, "请输入正确的身份证号格式");
        return false;
    }
    
    return true;
}

void EncryptionTestWidget::onEncryptClicked()
{
    if (!validateIdCard()) {
        return;
    }

    m_isEncrypting = true;
    m_encryptButton->setEnabled(false);
    m_encryptButton->setText("加密中...");

    m_statusText = "加密中";
    m_statusTag->setText(m_statusText);
    m_statusTag->setStyleSheet(QString(
                                   "font-size: 16px;"
                                   "padding: 8px 20px;"
                                   "border-radius: 4px;"
                                   "background: %1;"
                                   "color: white;"
                                   "border-bottom: none;"
                                   ).arg(getStatusColor(m_statusText)));

    QString idCard = m_inputData->text().trimmed();
    m_originalDataLabel->setText(idCard);

    qDebug() << "========================================";
    qDebug() << "开始加密测试";
    qDebug() << "输入身份证号：" << idCard;

    // 🔥 使用本地 PSI 加密
    QStringList idCards;
    idCards.append(idCard);

    QString contextData;
    QString payloadData;

    bool success = m_cryptoWrapper.encryptIdCards(idCards, contextData, payloadData);

    if (success) {
        m_isEncrypting = false;
        m_encryptButton->setEnabled(true);
        m_encryptButton->setText("加密");

        m_statusText = "加密成功";
        m_statusTag->setText(m_statusText);
        m_statusTag->setStyleSheet(QString(
                                       "font-size: 16px;"
                                       "padding: 8px 20px;"
                                       "border-radius: 4px;"
                                       "background: %1;"
                                       "color: white;"
                                       "border-bottom: none;"
                                       ).arg(getStatusColor(m_statusText)));

        // 🔥 显示加密结果
        // 🔥 友好的分段显示
        QString displayText;
        QTextStream stream(&displayText);

        stream << "【密文数据（Payload）】" << "\n";
        stream << "大小：" << payloadData.size() << " 字节" << "\n";
        stream << "内容：" << "\n";
        stream << payloadData.left(1000);  // 显示前500字符
        if (payloadData.size() > 1000) {
            stream << "\n... (共 " << payloadData.size() << " 字符)";
        }
        stream << "\n\n";

        // stream << "【上下文数据（Context）】" << "\n";
        // stream << "大小：" << contextData.size() << " 字节" << "\n";
        // stream << "内容：" << "\n";
        // stream << contextData.left(500);  // 显示前500字符
        // if (contextData.size() > 500) {
        //     stream << "\n... (共 " << contextData.size() << " 字符)";
        // }
        // stream << "\n\n";

        m_encryptedDataBox->setText(displayText);

        qDebug() << "加密成功";
        qDebug() << "Context大小：" << contextData.size();
        qDebug() << "Payload大小：" << payloadData.size();
        qDebug() << "身份证哈希：" << CryptoWrapper::hashIdCard(idCard);
        qDebug() << "========================================";

        MessageHelper::showSuccess(this, "身份证号加密成功");

    } else {
        m_isEncrypting = false;
        m_encryptButton->setEnabled(true);
        m_encryptButton->setText("加密");

        m_statusText = "加密失败";
        m_statusTag->setText(m_statusText);
        m_statusTag->setStyleSheet(QString(
                                       "font-size: 16px;"
                                       "padding: 8px 20px;"
                                       "border-radius: 4px;"
                                       "background: %1;"
                                       "color: white;"
                                       "border-bottom: none;"
                                       ).arg(getStatusColor(m_statusText)));

        m_encryptedDataBox->setText("加密失败");

        qDebug() << "加密失败";
        qDebug() << "========================================";

        MessageHelper::showError(this, "身份证号加密失败");
    }
}

QString EncryptionTestWidget::getStatusColor(const QString& status)
{
    if (status == "加密成功") return "#67c23a";
    if (status == "加密中") return "#e6a23c";
    if (status == "加密失败") return "#f56c6c";
    return "#909399";
}

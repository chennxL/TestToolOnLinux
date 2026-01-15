#include "networkrequest.h"
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <QUrl>

NetworkRequest::NetworkRequest(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8080")
{
}

NetworkRequest::~NetworkRequest()
{
}

NetworkRequest& NetworkRequest::instance()
{
    static NetworkRequest instance;
    return instance;
}

void NetworkRequest::setBaseUrl(const QString& url)
{
    m_baseUrl = url;
}

void NetworkRequest::get(const QString& url,
                        std::function<void(const QJsonObject&)> onSuccess,
                        std::function<void(const QString&)> onError,
                        int timeout)
{
    QString fullUrl = url.startsWith("http") ? url : m_baseUrl + url;
    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply* reply = m_networkManager->get(request);
    
    // 设置超时
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [reply, onError, timer]() {
        if (reply->isRunning()) {
            reply->abort();
            onError("请求超时");
        }
        timer->deleteLater();
    });
    timer->start(timeout);
    
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError, timer]() {
        timer->stop();
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkRequest::post(const QString& url,
                         const QJsonObject& data,
                         std::function<void(const QJsonObject&)> onSuccess,
                         std::function<void(const QString&)> onError,
                         int timeout)
{
    QString fullUrl = url.startsWith("http") ? url : m_baseUrl + url;
    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    QNetworkReply* reply = m_networkManager->post(request, jsonData);
    
    // 设置超时
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [reply, onError, timer]() {
        if (reply->isRunning()) {
            reply->abort();
            onError("请求超时");
        }
        timer->deleteLater();
    });
    timer->start(timeout);
    
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError, timer]() {
        timer->stop();
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkRequest::downloadFile(const QString& url,
                                 std::function<void(const QByteArray&, const QString&)> onSuccess,
                                 std::function<void(const QString&)> onError,
                                 int timeout)
{
    QString fullUrl = url.startsWith("http") ? url : m_baseUrl + url;
    QNetworkRequest request(fullUrl);
    
    QNetworkReply* reply = m_networkManager->get(request);
    
    // 设置超时
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [reply, onError, timer]() {
        if (reply->isRunning()) {
            reply->abort();
            onError("请求超时");
        }
        timer->deleteLater();
    });
    timer->start(timeout);
    
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError, timer]() {
        timer->stop();
        handleFileReply(reply, onSuccess, onError);
    });
}
void NetworkRequest::handleReply(QNetworkReply* reply,
                                 std::function<void(const QJsonObject&)> onSuccess,
                                 std::function<void(const QString&)> onError)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        qWarning() << "Network error:" << errorMsg;
        qWarning() << "HTTP status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // qWarning() << "Response body:" << reply->readAll();
        onError(QString("网络错误: %1").arg(errorMsg));
        return;
    }

    QByteArray responseData = reply->readAll();
    // qDebug() << "Response data:" << responseData;

    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (doc.isNull() || !doc.isObject()) {
        QString errorMsg = QString("无效的响应数据。原始响应: %1").arg(QString::fromUtf8(responseData));
        qWarning() << errorMsg;
        onError(errorMsg);
        return;
    }

    QJsonObject jsonObj = doc.object();

    // 检查业务逻辑错误
    int code = jsonObj.value("code").toInt();
    if (code != 200) {
        QString message = jsonObj.value("message").toString("未知错误");
        QString errorMsg = QString("服务器错误 (code=%1): %2").arg(code).arg(message);
        qWarning() << errorMsg;
        onError(errorMsg);
        return;
    }

    onSuccess(jsonObj);
}

void NetworkRequest::handleFileReply(QNetworkReply* reply,
                                    std::function<void(const QByteArray&, const QString&)> onSuccess,
                                    std::function<void(const QString&)> onError)
{
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        qWarning() << "Network error:" << errorMsg;
        onError(errorMsg);
        return;
    }
    
    // 检查Content-Type是否为JSON（错误响应）
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (contentType.contains("application/json")) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject jsonObj = doc.object();
            QString message = jsonObj.value("message").toString("下载失败");
            onError(message);
            return;
        }
    }
    
    // 获取文件数据
    QByteArray fileData = reply->readAll();
    
    // 尝试从Content-Disposition头获取文件名
    QString filename;
    QString contentDisposition = reply->rawHeader("Content-Disposition");
    if (!contentDisposition.isEmpty()) {
        QRegularExpression re("filename[^;=\\n]*=((['\"]).*?\\2|[^;\\n]*)");
        QRegularExpressionMatch match = re.match(contentDisposition);
        if (match.hasMatch()) {
            filename = match.captured(1).remove("\"").remove("'");
            filename = QUrl::fromPercentEncoding(filename.toUtf8());
        }
    }
    
    if (filename.isEmpty()) {
        filename = QString("测试集查询结果_%1.xlsx")
                      .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
    }
    
    onSuccess(fileData, filename);
}

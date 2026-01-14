#include "apiservice.h"
#include "networkrequest.h"

ApiService::ApiService(QObject *parent)
    : QObject(parent)
{
}

ApiService::~ApiService()
{
}

ApiService& ApiService::instance()
{
    static ApiService instance;
    return instance;
}

// ============ 黑名单API ============

void ApiService::createBlacklist(int size,
                                std::function<void(const QJsonObject&)> onSuccess,
                                std::function<void(const QString&)> onError)
{
    QJsonObject data;
    data["size"] = size;
    
    NetworkRequest::instance().post("/blacklist/create", data, onSuccess, onError, 30000);
}

void ApiService::getBlacklistStatus(std::function<void(const QJsonObject&)> onSuccess,
                                   std::function<void(const QString&)> onError)
{
    NetworkRequest::instance().get("/blacklist/status", onSuccess, onError);
}

// 🔥 新增:查询黑名单数量
void ApiService::getBlacklistCount(std::function<void(const QJsonObject&)> onSuccess,
                                   std::function<void(const QString&)> onError)
{
    NetworkRequest::instance().get("/blacklist/count", onSuccess, onError);
}

// ============ 测试集API ============

void ApiService::createTestSet(int insideSize, int outsideSize,
                              std::function<void(const QJsonObject&)> onSuccess,
                              std::function<void(const QString&)> onError)
{
    QJsonObject data;
    data["insideSize"] = insideSize;
    data["outsideSize"] = outsideSize;
    
    NetworkRequest::instance().post("/testset/create", data, onSuccess, onError, 30000);
}

void ApiService::saveEncryptedData(const QString& payload,
                                   const QString& context,
                                   std::function<void(const QJsonObject&)> onSuccess,
                                   std::function<void(const QString&)> onError)
{
    QJsonObject requestBody;
    requestBody["payloadData"] = payload;      // 改为 payloadData
    requestBody["contextData"] = context;      // 改为 contextData

    NetworkRequest::instance().post("/testset/saveEncrypted", requestBody, onSuccess, onError, 30000);
}

void ApiService::queryBlacklistWithData(const QString& payload,
                                        const QString& context,
                                        std::function<void(const QJsonObject&)> onSuccess,
                                        std::function<void(const QString&)> onError)
{
    QJsonObject requestBody;
    requestBody["payloadData"] = payload;
    requestBody["contextData"] = context;

    qDebug() << "发送查询请求，数据大小 - payload:" << payload.size() << "context:" << context.size();

    // 查询操作可能耗时较长，设置3分钟超时
    NetworkRequest::instance().post("/testset/query", requestBody, onSuccess, onError, 1800000);
}

void ApiService::exportResults(std::function<void(const QByteArray&, const QString&)> onSuccess,
                              std::function<void(const QString&)> onError)
{
    NetworkRequest::instance().downloadFile("/testset/export", onSuccess, onError, 30000);
}

void ApiService::getTestSetStatus(std::function<void(const QJsonObject&)> onSuccess,
                                 std::function<void(const QString&)> onError)
{
    NetworkRequest::instance().get("/testset/status", onSuccess, onError);
}

// ============ 加密API ============

void ApiService::encryptIdCard(const QString& idCard,
                              std::function<void(const QString&)> onSuccess,
                              std::function<void(const QString&)> onError)
{
    QJsonObject data;
    data["idCard"] = idCard;
    
    NetworkRequest::instance().post("/encryption/encrypt", data,
        [onSuccess](const QJsonObject& response) {
            // 提取data字段（加密后的字符串）
            QString encryptedData = response.value("data").toString();
            onSuccess(encryptedData);
        },
        onError,
        30000);
}

void ApiService::decryptIdCard(const QString& encryptedIdCard,
                              std::function<void(const QString&)> onSuccess,
                              std::function<void(const QString&)> onError)
{
    QJsonObject data;
    data["encryptedIdCard"] = encryptedIdCard;
    
    NetworkRequest::instance().post("/encryption/decrypt", data,
        [onSuccess](const QJsonObject& response) {
            // 提取data字段（解密后的字符串）
            QString decryptedData = response.value("data").toString();
            onSuccess(decryptedData);
        },
        onError,
        30000);
}

#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <functional>

class ApiService : public QObject
{
    Q_OBJECT

public:
    static ApiService& instance();
    
    // 黑名单API
    void createBlacklist(int size,
                        std::function<void(const QJsonObject&)> onSuccess,
                        std::function<void(const QString&)> onError);
    
    void getBlacklistStatus(std::function<void(const QJsonObject&)> onSuccess,
                           std::function<void(const QString&)> onError);
    
    // 🔥 新增:查询黑名单数量
    void getBlacklistCount(std::function<void(const QJsonObject&)> onSuccess,
                           std::function<void(const QString&)> onError);

    // 测试集API
    void createTestSet(int insideSize, int outsideSize,
                      std::function<void(const QJsonObject&)> onSuccess,
                      std::function<void(const QString&)> onError);

    void saveEncryptedData(const QString& payload,
                           const QString& context,
                           std::function<void(const QJsonObject&)> onSuccess,
                           std::function<void(const QString&)> onError);
    
    void queryBlacklistWithData(const QString& payload,
                                const QString& context,
                                std::function<void(const QJsonObject&)> onSuccess,
                                std::function<void(const QString&)> onError);

    void exportResults(std::function<void(const QByteArray&, const QString&)> onSuccess,
                      std::function<void(const QString&)> onError);
    
    void getTestSetStatus(std::function<void(const QJsonObject&)> onSuccess,
                         std::function<void(const QString&)> onError);
    
    // 加密API
    void encryptIdCard(const QString& idCard,
                      std::function<void(const QString&)> onSuccess,
                      std::function<void(const QString&)> onError);
    
    void decryptIdCard(const QString& encryptedIdCard,
                      std::function<void(const QString&)> onSuccess,
                      std::function<void(const QString&)> onError);

private:
    explicit ApiService(QObject *parent = nullptr);
    ~ApiService();
    ApiService(const ApiService&) = delete;
    ApiService& operator=(const ApiService&) = delete;
};

#endif // APISERVICE_H

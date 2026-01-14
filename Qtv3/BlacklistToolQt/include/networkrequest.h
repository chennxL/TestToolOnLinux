#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <functional>

class NetworkRequest : public QObject
{
    Q_OBJECT

public:
    static NetworkRequest& instance();
    
    // HTTP请求方法
    void get(const QString& url, 
             std::function<void(const QJsonObject&)> onSuccess,
             std::function<void(const QString&)> onError,
             int timeout = 30000);
    
    void post(const QString& url, 
              const QJsonObject& data,
              std::function<void(const QJsonObject&)> onSuccess,
              std::function<void(const QString&)> onError,
              int timeout = 30000);
    
    // 下载文件（用于导出Excel）
    void downloadFile(const QString& url,
                     std::function<void(const QByteArray&, const QString&)> onSuccess,
                     std::function<void(const QString&)> onError,
                     int timeout = 30000);
    
    void setBaseUrl(const QString& url);
    QString baseUrl() const { return m_baseUrl; }

private:
    explicit NetworkRequest(QObject *parent = nullptr);
    ~NetworkRequest();
    NetworkRequest(const NetworkRequest&) = delete;
    NetworkRequest& operator=(const NetworkRequest&) = delete;
    
    void handleReply(QNetworkReply* reply,
                    std::function<void(const QJsonObject&)> onSuccess,
                    std::function<void(const QString&)> onError);
    
    void handleFileReply(QNetworkReply* reply,
                        std::function<void(const QByteArray&, const QString&)> onSuccess,
                        std::function<void(const QString&)> onError);
    
    QNetworkAccessManager* m_networkManager;
    QString m_baseUrl;
};

#endif // NETWORKREQUEST_H

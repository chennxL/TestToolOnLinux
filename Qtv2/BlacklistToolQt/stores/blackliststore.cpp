#include "blackliststore.h"
#include "apiservice.h"
#include <QDebug>

BlacklistStore::BlacklistStore(QObject *parent)
    : QObject(parent)
    , m_status(NotCreated)
    , m_size(0)
{
}

BlacklistStore::~BlacklistStore()
{
}

BlacklistStore& BlacklistStore::instance()
{
    static BlacklistStore instance;
    return instance;
}

QString BlacklistStore::statusText() const
{
    switch (m_status) {
    case NotCreated:
        return "未创建";
    case Creating:
        return "创建中";
    case Created:
        return "已创建";
    case CreateFailed:
        return "创建失败";
    default:
        return "未知";
    }
}

void BlacklistStore::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

void BlacklistStore::setSize(int size)
{
    if (m_size != size) {
        m_size = size;
        emit sizeChanged(size);
    }
}
void BlacklistStore::createBlacklist(int size)
{
    qDebug() << "Creating blacklist with size:" << size;
    setStatus(Creating);

    ApiService::instance().createBlacklist(size,
   [this, size](const QJsonObject& response) {
       qDebug() << "Blacklist creation response:" << response;
       int code = response.value("code").toInt();
       if (code == 200) {
           setStatus(Created);
           setSize(size);
           emit createSuccess();
       } else {
           setStatus(CreateFailed);
           setSize(0);
           QString message = response.value("message").toString("创建失败");
           qWarning() << "Blacklist creation failed:" << message;
           emit createFailed(message);
       }
   },
   [this](const QString& error) {
       qWarning() << "Blacklist creation network error:" << error;
       setStatus(CreateFailed);
       setSize(0);
       emit createFailed(error);
    }
   );
}

void BlacklistStore::loadFromDatabase()
{
    qDebug() << "Loading blacklist from database...";
    
    ApiService::instance().getBlacklistCount(
        [this](const QJsonObject& response) {
            int code = response.value("code").toInt();
            if (code == 200) {
                // 从响应中获取数量
                int count = response.value("data").toInt(0);
                qDebug() << "Blacklist count from database:" << count;
                
                if (count > 0) {
                    // 如果数据库中有黑名单数据,设置为已创建状态
                    setStatus(Created);
                    setSize(count);
                    qDebug() << "Blacklist loaded successfully, size:" << count;
                } else {
                    // 数据库为空,保持未创建状态
                    setStatus(NotCreated);
                    setSize(0);
                    qDebug() << "No blacklist found in database";
                }
            } else {
                qWarning() << "Failed to load blacklist:" << response.value("message").toString();
                // 加载失败,保持未创建状态
                setStatus(NotCreated);
                setSize(0);
            }
        },
        [this](const QString& error) {
            qWarning() << "Failed to load blacklist:" << error;
            // 网络错误,保持未创建状态
            setStatus(NotCreated);
            setSize(0);
        }
    );
}

void BlacklistStore::reset()
{
    setStatus(NotCreated);
    setSize(0);
}

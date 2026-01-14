#ifndef BLACKLISTSTORE_H
#define BLACKLISTSTORE_H

#include <QObject>
#include <QString>

class BlacklistStore : public QObject
{
    Q_OBJECT

public:
    static BlacklistStore& instance();
    
    // 状态枚举
    enum Status {
        NotCreated,     // 未创建
        Creating,       // 创建中
        Created,        // 已创建
        CreateFailed    // 创建失败
    };
    
    // Getter
    Status status() const { return m_status; }
    QString statusText() const;
    int size() const { return m_size; }
    
    // Setter
    void setStatus(Status status);
    void setSize(int size);
    
    // 业务方法
    void createBlacklist(int size);
    void loadFromDatabase();  // 🔥 新增:从数据库加载黑名单信息
    void reset();

signals:
    void statusChanged(Status status);
    void sizeChanged(int size);
    void createSuccess();
    void createFailed(const QString& error);

private:
    explicit BlacklistStore(QObject *parent = nullptr);
    ~BlacklistStore();
    BlacklistStore(const BlacklistStore&) = delete;
    BlacklistStore& operator=(const BlacklistStore&) = delete;
    
    Status m_status;
    int m_size;
};

#endif // BLACKLISTSTORE_H

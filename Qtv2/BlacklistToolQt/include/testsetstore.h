#ifndef TESTSETSTORE_H
#define TESTSETSTORE_H

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include "cryptowrapper.h"  // 添加这一行
#include "blacklistinfo.h"

class TestSetStore : public QObject
{
    Q_OBJECT

public:
    static TestSetStore& instance();
    
    // 测试集状态枚举
    enum TestSetStatus {
        NotCreated,     // 未创建
        Creating,       // 创建中
        Created,        // 已创建
        CreateFailed    // 创建失败
    };
    
    // 查询状态枚举
    enum QueryStatus {
        NotExecuted,    // 未执行
        Querying,       // 查询中
        QueryCompleted, // 查询完成
        QueryFailed     // 查询失败
    };
    
    // Getter - 测试集
    TestSetStatus testSetStatus() const { return m_testSetStatus; }
    QString testSetStatusText() const;
    int insideCount() const { return m_insideCount; }
    int outsideCount() const { return m_outsideCount; }
    
    // Getter - 查询
    QueryStatus queryStatus() const { return m_queryStatus; }
    QString queryStatusText() const;
    int matchCount() const { return m_matchCount; }
    int totalCount() const { return m_totalCount; }
    double queryTime() const { return m_queryTime; }
    
    // Setter
    void setTestSetStatus(TestSetStatus status);
    void setTestSetSize(int inside, int outside);
    void setQueryStatus(QueryStatus status);
    void setQueryResult(int matched, int total, double time);
    
    // 业务方法
    void createTestSet(int insideSize, int outsideSize);
    void queryBlacklist();
    void exportResults();
    void reset();

signals:
    void testSetStatusChanged(TestSetStatus status);
    void testSetSizeChanged(int inside, int outside);
    void queryStatusChanged(QueryStatus status);
    void queryResultChanged(int matched, int total, double time);
    
    void testSetCreateSuccess();
    void testSetCreateFailed(const QString& error);
    void querySuccess();
    void queryFailed(const QString& error);
    void exportSuccess(const QString& filename);
    void exportFailed(const QString& error);

private:
    QString m_cachedContextData;   // 缓存的上下文数据
    QString m_cachedPayloadData;   // 缓存的负载数据
    QString m_cachedEncryptedResult; // 缓存加密的查询结果（用于解密）
    explicit TestSetStore(QObject *parent = nullptr);
    ~TestSetStore();
    TestSetStore(const TestSetStore&) = delete;
    TestSetStore& operator=(const TestSetStore&) = delete;

    // 测试集状态
    TestSetStatus m_testSetStatus;
    int m_insideCount;
    int m_outsideCount;

    // 待创建的测试集大小（用于异步创建）
    int m_pendingInsideSize;
    int m_pendingOutsideSize;

    // 查询状态
    QueryStatus m_queryStatus;
    int m_matchCount;
    int m_totalCount;
    double m_queryTime;

    // 查询开始时间（用于计算耗时）
    qint64 m_queryStartTime;

    // 加密工具
    CryptoWrapper m_cryptoWrapper;  // 添加这一行

    QVector<MatchedBlacklistInfo> m_matchedInfoList;  // 存储匹配的完整信息
    // 🔥 新增：保存原始测试集数据
    QStringList m_originalTestSet;       // 原始测试集（所有身份证号）
    QSet<QString> m_insideIdCards;       // 库内身份证号集合（用于判断）
    QMap<size_t, QString> m_insideIdCardHashes;  // 库内身份证号的哈希映射
};
#endif // TESTSETSTORE_H

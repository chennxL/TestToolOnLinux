#ifndef CRYPTOWRAPPER_H
#define CRYPTOWRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>
#include "blacklistinfo.h"  // 新增

// 前向声明
struct Client_Context_t;
struct Reveal_Table;

class CryptoWrapper : public QObject
{
    Q_OBJECT
public:
    explicit CryptoWrapper(QObject *parent = nullptr);
    ~CryptoWrapper();

    /**
     * 将身份证号转换为size_t类型的key
     * 使用SHA256哈希算法
     */
    static size_t hashIdCard(const QString& idCard);

    /**
     * 加密身份证号列表
     * @param idCards 身份证号列表
     * @param contextOut 输出：Base64编码的上下文数据
     * @param payloadOut 输出：Base64编码的加密负载
     * @return 成功返回true
     */
    bool encryptIdCards(const QStringList& idCards,
                        QString& contextOut,
                        QString& payloadOut);

    /**
     * 解密查询结果，返回匹配的身份证号列表（旧版本，兼容用）
     * @param encryptedResult Base64编码的加密结果
     * @param matchedIdCards 输出：匹配的身份证号列表
     * @return 成功返回true
     */
    bool decryptResult(const QString& encryptedResult,
                       QStringList& matchedIdCards);

    /**
     * 解密查询结果，返回完整的黑名单信息（新版本）
     * @param encryptedResult Base64编码的加密结果
     * @param matchedInfoList 输出：匹配的黑名单完整信息列表
     * @return 成功返回true
     */
    bool decryptResultWithDetails(const QString& encryptedResult,
                                  QVector<MatchedBlacklistInfo>& matchedInfoList);

private:
    Client_Context_t* m_context;
    Reveal_Table* m_revealTable;

    // 保存哈希值到身份证号的映射，用于解密后还原
    QMap<size_t, QString> m_hashToIdCardMap;
};

#endif // CRYPTOWRAPPER_H

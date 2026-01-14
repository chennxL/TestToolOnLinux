#include "cryptowrapper.h"
#include "blacklistbitdecoder.h"
#include <QDebug>
#include <QCryptographicHash>
#include <vector>

// 直接包含头文件，不需要 extern "C"
// 因为 psicommon.h 和 psiclient.h 内部已经处理了 C/C++ 兼容性
#include "psicommon.h"
#include "psiclient.h"

CryptoWrapper::CryptoWrapper(QObject *parent)
    : QObject(parent)
    , m_context(nullptr)
    , m_revealTable(nullptr)
{
}

CryptoWrapper::~CryptoWrapper()
{
    // 清理reveal_table
    if (m_revealTable) {
        PSI_Reveal_Table_Destory(m_revealTable);
        m_revealTable = nullptr;
    }

    // 清理context
    if (m_context) {
        PSI_Client_Context_Destory(m_context);
        m_context = nullptr;
    }
}

size_t CryptoWrapper::hashIdCard(const QString& idCard)
{
    // 将身份证号转换为size_t类型的key
    // 使用SHA256哈希，取前8字节
    QByteArray hash = QCryptographicHash::hash(
        idCard.toUtf8(),
        QCryptographicHash::Sha256
        );

    // 取前8字节作为size_t
    size_t result = 0;
    for (int i = 0; i < 8 && i < hash.size(); ++i) {
        result = (result << 8) | static_cast<unsigned char>(hash[i]);
    }
    return result;
}

bool CryptoWrapper::encryptIdCards(const QStringList& idCards,
                                   QString& contextOut,
                                   QString& payloadOut)
{
    try {
        qDebug() << "开始加密，数据量：" << idCards.size();

        // 清空之前的映射表
        m_hashToIdCardMap.clear();

        // 1. 将身份证号转换为size_t数组，并保存映射关系
        std::vector<size_t> cli_data;
        cli_data.reserve(idCards.size());

        for (const QString& idCard : idCards) {
            size_t key = hashIdCard(idCard);
            cli_data.push_back(key);

            // 保存映射关系：哈希值 → 身份证号
            m_hashToIdCardMap[key] = idCard;

            qDebug() << "身份证号:" << idCard << "-> 哈希值:" << key;
        }

        qDebug() << "数据准备完成，实际数据量：" << cli_data.size();
        qDebug() << "映射表大小：" << m_hashToIdCardMap.size();

        // 2. 清理旧的上下文和reveal_table（如果存在）
        if (m_revealTable) {
            PSI_Reveal_Table_Destory(m_revealTable);
            m_revealTable = nullptr;
        }
        if (m_context) {
            PSI_Client_Context_Destory(m_context);
            m_context = nullptr;
        }

        // 3. 创建客户端上下文（参数：15, 16, 14）
        m_context = PSI_Client_Context_Create(15, 16, 14);
        if (!m_context) {
            qWarning() << "创建客户端上下文失败";
            return false;
        }
        qDebug() << "客户端上下文创建成功";

        // 4. 生成并序列化上下文
        C_Stream* ctx_stream = PSI_Client_Context_To_Stream(m_context);
        if (!ctx_stream) {
            qWarning() << "序列化上下文失败";
            return false;
        }

        size_t ctx_len = 0;
        const char* ctx_data = PSI_Stream_Read(ctx_stream, &ctx_len);
        QByteArray contextData(ctx_data, static_cast<int>(ctx_len));
        contextOut = QString::fromLatin1(contextData.toBase64());
        qDebug() << "上下文序列化完成，大小：" << ctx_len;

        PSI_Stream_Destroy(ctx_stream);

        // 5. 加密查询内容
        // 第三个参数是元素个数
        C_Stream* payload_stream = PSI_Client_Pack_Payload(
            m_context,
            cli_data.data(),
            cli_data.size(),  // 元素个数
            &m_revealTable
            );

        if (!payload_stream) {
            qWarning() << "加密数据失败";
            return false;
        }
        qDebug() << "数据加密完成";

        // 6. 读取payload
        size_t payload_len = 0;
        const char* payload_data = PSI_Stream_Read(payload_stream, &payload_len);
        QByteArray payloadBytes(payload_data, static_cast<int>(payload_len));
        payloadOut = QString::fromLatin1(payloadBytes.toBase64());
        qDebug() << "加密数据序列化完成，大小：" << payload_len;

        PSI_Stream_Destroy(payload_stream);

        // 注意：m_context、m_revealTable 和 m_hashToIdCardMap 保留，用于后续解密

        return true;
    } catch (const std::exception& e) {
        qWarning() << "加密失败：" << e.what();
        return false;
    } catch (...) {
        qWarning() << "加密失败：未知错误";
        return false;
    }
}

bool CryptoWrapper::decryptResult(const QString& encryptedResult,
                                  QStringList& matchedIdCards)
{
    // 调用新方法获取完整信息
    QVector<MatchedBlacklistInfo> matchedInfoList;
    if (!decryptResultWithDetails(encryptedResult, matchedInfoList)) {
        return false;
    }

    // 只提取身份证号
    matchedIdCards.clear();
    for (const auto& info : matchedInfoList) {
        matchedIdCards.append(info.idCard);
    }

    return true;
}
bool CryptoWrapper::decryptResultWithDetails(const QString& encryptedResult,
                                             QVector<MatchedBlacklistInfo>& matchedInfoList)
{
    try {
        if (!m_context || !m_revealTable) {
            qWarning() << "解密失败：缺少上下文或reveal_table";
            return false;
        }

        qDebug() << "========================================";
        qDebug() << "开始解密结果";
        qDebug() << "映射表大小：" << m_hashToIdCardMap.size();

        // 1. Base64解码
        QByteArray encryptedData = QByteArray::fromBase64(encryptedResult.toLatin1());
        qDebug() << "加密数据大小：" << encryptedData.size();

        // 2. 解密匹配结果
        size_t result_count = 0;
        Reveal_Result* results = PSI_Client_Reveal_Result(
            m_context,
            m_revealTable,
            encryptedData.data(),
            static_cast<size_t>(encryptedData.size()),
            &result_count
            );

        if (!results) {
            qWarning() << "解密失败：PSI_Client_Reveal_Result返回NULL";
            return false;
        }

        qDebug() << "解密成功，结果数量：" << result_count;
        qDebug() << "----------------------------------------";

        // 3. 解析每个结果
        matchedInfoList.clear();

        for (size_t i = 0; i < result_count; ++i) {
            size_t key = results[i].key;
            size_t value_count = results[i].count;
            size_t* values = results[i].value;

            qDebug() << "结果[" << i << "]:";
            qDebug() << "  key(身份证哈希):" << key;
            qDebug() << "  labels数量:" << value_count;

            // 打印所有labels
            qDebug() << "  所有labels:";
            for (size_t j = 0; j < value_count; ++j) {
                qDebug() << "    labels[" << j << "]:" << values[j]
                         << "(0x" << QString::number(values[j], 16) << ")";
            }

            // 通过映射表找回原始身份证号
            if (!m_hashToIdCardMap.contains(key)) {
                qWarning() << "  警告：找不到key对应的身份证号，跳过";
                continue;
            }

            QString idCard = m_hashToIdCardMap[key];
            qDebug() << "  身份证号:" << idCard;

            // 检查labels数组是否有数据
            if (value_count == 0 || !values) {
                qWarning() << "  警告：labels数组为空";
                continue;
            }

            // 🔥 将所有labels转换为QVector
            QVector<uint64_t> labelsVector;
            for (size_t j = 0; j < value_count; ++j) {
                labelsVector.append(values[j]);
            }

            // 使用多labels解码器解码
            MatchedBlacklistInfo info = BlacklistBitDecoder::decodeFromLabels(labelsVector);

            // 设置身份证信息
            info.idCard = idCard;
            info.idCardHash = key;

            matchedInfoList.append(info);
        }

        qDebug() << "----------------------------------------";
        qDebug() << "最终匹配数量：" << matchedInfoList.size();
        qDebug() << "========================================";

        // 4. 释放结果
        PSI_Reveal_Result_Destory(results);

        return true;

    } catch (const std::exception& e) {
        qWarning() << "解密失败：" << e.what();
        return false;
    } catch (...) {
        qWarning() << "解密失败：未知错误";
        return false;
    }
}

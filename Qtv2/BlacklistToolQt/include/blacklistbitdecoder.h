#ifndef BLACKLISTBITDECODER_H
#define BLACKLISTBITDECODER_H

#include "blacklistinfo.h"
#include <cstdint>
#include <QDebug>
#include <QVector>

/**
 * @brief 黑名单信息位解码工具（多labels方案）
 *
 * 编码格式：
 * labels[0]: [3-0]位=行为评级, [7-4]位=行为记录数
 * labels[1]: [3-0]位=记录1行为类型, [7-4]位=记录1使用工具
 * labels[2]: [3-0]位=记录2行为类型, [7-4]位=记录2使用工具
 * labels[3]: [3-0]位=记录3行为类型, [7-4]位=记录3使用工具
 */
class BlacklistBitDecoder {
public:
    /**
     * @brief 从labels数组解码出完整的黑名单信息
     * @param labels labels数组（至少1个元素）
     * @return 解码后的信息
     */
    static MatchedBlacklistInfo decodeFromLabels(const QVector<uint64_t>& labels) {
        MatchedBlacklistInfo info;
        info.idCardHash = 0;

        if (labels.isEmpty()) {
            qWarning() << "labels数组为空！";
            return info;
        }

        // 解码labels[0]: 行为评级 + 记录数
        uint64_t label0 = labels[0];
        info.riskLevel = static_cast<int>(label0 & 0xF);
        info.recordCount = static_cast<int>((label0 >> 4) & 0xF);

        qDebug() << "  解码labels[0]:" << label0;
        qDebug() << "    评级=" << info.riskLevel << "(" << info.riskLevelDesc() << ")";
        qDebug() << "    记录数=" << info.recordCount;

        // 解码每条行为记录
        int actualRecords = qMin(info.recordCount, labels.size() - 1);
        for (int i = 0; i < actualRecords; i++) {
            uint64_t recordLabel = labels[i + 1];

            BehaviorRecordInfo record;
            record.behaviorType = static_cast<int>(recordLabel & 0xF);
            record.toolType = static_cast<int>((recordLabel >> 4) & 0xF);

            info.records.append(record);

            qDebug() << "  解码labels[" << (i + 1) << "]:" << recordLabel;
            qDebug() << "    行为类型=" << record.behaviorType
                     << "(" << record.behaviorTypeDesc() << ")";
            qDebug() << "    使用工具=" << record.toolType
                     << "(" << record.toolTypeDesc() << ")";
        }

        return info;
    }

    /**
     * @brief 解码labels[0]获取行为评级
     */
    static int decodeRiskLevel(uint64_t label0) {
        return static_cast<int>(label0 & 0xF);
    }

    /**
     * @brief 解码labels[0]获取行为记录数
     */
    static int decodeRecordCount(uint64_t label0) {
        return static_cast<int>((label0 >> 4) & 0xF);
    }

    /**
     * @brief 解码行为记录label
     * @return [行为类型, 使用工具]
     */
    static BehaviorRecordInfo decodeBehaviorLabel(uint64_t label) {
        BehaviorRecordInfo record;
        record.behaviorType = static_cast<int>(label & 0xF);
        record.toolType = static_cast<int>((label >> 4) & 0xF);
        return record;
    }
};

#endif // BLACKLISTBITDECODER_H

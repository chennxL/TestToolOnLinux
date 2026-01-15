#ifndef BLACKLISTINFO_H
#define BLACKLISTINFO_H

#include <QString>
#include <QVector>
#include <cstdint>

/**
 * @brief 行为记录信息
 */
struct BehaviorRecordInfo {
    int behaviorType;  // 行为类型 (1-藏匿, 2-投诉, 3-其他)
    int toolType;      // 使用工具 (1-刀具, 2-打火机, 3-其他)

    QString behaviorTypeDesc() const {
        switch (behaviorType) {
        case 1: return "藏匿";
        case 2: return "投诉";
        case 3: return "其他";
        default: return "未知";
        }
    }

    QString toolTypeDesc() const {
        switch (toolType) {
        case 1: return "刀具";
        case 2: return "打火机";
        case 3: return "其他";
        default: return "未知";
        }
    }
};

/**
 * @brief 匹配结果完整信息
 */
struct MatchedBlacklistInfo {
    QString idCard;                          // 身份证号
    uint64_t idCardHash;                     // 身份证哈希值
    int riskLevel;                           // 行为评级 (1-A, 2-B, 3-C)
    int recordCount;                         // 行为记录数 (1-3)
    QVector<BehaviorRecordInfo> records;     // 行为记录列表

    QString riskLevelDesc() const {
        switch (riskLevel) {
        case 1: return "A";
        case 2: return "B";
        case 3: return "C";
        default: return "未知";
        }
    }
};

#endif // BLACKLISTINFO_H

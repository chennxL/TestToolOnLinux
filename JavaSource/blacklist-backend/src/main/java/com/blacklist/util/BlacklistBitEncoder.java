package com.blacklist.util;

import com.blacklist.dto.BlacklistFullInfo;
import com.blacklist.entity.BehaviorRecord;
import com.blacklist.entity.BlacklistMain;
import lombok.extern.slf4j.Slf4j;

import java.util.List;

/**
 * 黑名单信息位编码工具（多labels方案）
 *
 * 编码格式：
 * labels[0]: [3-0]位=行为评级, [7-4]位=行为记录数
 * labels[1]: [3-0]位=记录1行为类型, [7-4]位=记录1使用工具
 * labels[2]: [3-0]位=记录2行为类型, [7-4]位=记录2使用工具
 * labels[3]: [3-0]位=记录3行为类型, [7-4]位=记录3使用工具
 */
@Slf4j
public class BlacklistBitEncoder {

    /**
     * 将黑名单完整信息编码为labels数组
     * @return long数组，每个元素对应一个label
     */
    public static long[] encodeBlacklistInfoToLabels(BlacklistFullInfo info) {
        BlacklistMain main = info.getMain();
        List<BehaviorRecord> records = info.getRecords();

        // labels数组长度 = 1(基本信息) + 实际记录数
        int labelsCount = 1 + records.size();
        long[] labels = new long[labelsCount];

        // labels[0]: 行为评级(低4位) + 行为记录数(高4位)
        long riskLevel = main.getRiskLevel().getCode().longValue();
        long recordCount = main.getRecordCount().longValue();
        labels[0] = (riskLevel & 0xF) | ((recordCount & 0xF) << 4);

        // labels[1-3]: 每条行为记录
        for (int i = 0; i < records.size(); i++) {
            BehaviorRecord record = records.get(i);
            long behaviorType = record.getBehaviorType().getCode().longValue();
            long toolType = record.getTool().getCode().longValue();

            // 行为类型(低4位) + 使用工具(高4位)
            labels[i + 1] = (behaviorType & 0xF) | ((toolType & 0xF) << 4);
        }

        return labels;
    }

    /**
     * 解码labels[0]获取行为评级
     */
    public static int decodeRiskLevel(long label0) {
        return (int) (label0 & 0xF);
    }

    /**
     * 解码labels[0]获取行为记录数
     */
    public static int decodeRecordCount(long label0) {
        return (int) ((label0 >> 4) & 0xF);
    }

    /**
     * 解码行为记录label（labels[1-3]）
     * @return [行为类型, 使用工具]
     */
    public static int[] decodeBehaviorLabel(long label) {
        int behaviorType = (int) (label & 0xF);
        int toolType = (int) ((label >> 4) & 0xF);
        return new int[]{behaviorType, toolType};
    }

    /**
     * 打印编码信息（调试用）
     */
    public static void printLabels(long[] labels) {
        log.info("=== Labels编码信息 ===");
        log.info("labels.length: {}", labels.length);

        if (labels.length > 0) {
            log.info("labels[0]: {} (评级={}, 记录数={})",
                    labels[0],
                    decodeRiskLevel(labels[0]),
                    decodeRecordCount(labels[0]));
        }

        for (int i = 1; i < labels.length; i++) {
            int[] decoded = decodeBehaviorLabel(labels[i]);
            log.info("labels[{}]: {} (行为类型={}, 使用工具={})",
                    i, labels[i], decoded[0], decoded[1]);
        }

        log.info("====================");
    }
}
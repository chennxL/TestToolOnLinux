package com.blacklist.util;

import com.blacklist.enums.BehaviorTypeEnum;
import com.blacklist.enums.ToolTypeEnum;

import java.time.LocalDateTime;
import java.util.Random;

/**
 * 行为记录生成工具类
 */
/**
 * 行为记录生成工具类
 */
public class BehaviorGenerator {

    private static final Random RANDOM = new Random();

    /**
     * 生成随机行为类型（枚举）
     */
    public static BehaviorTypeEnum generateBehaviorType() {
        BehaviorTypeEnum[] types = BehaviorTypeEnum.values();
        return types[RANDOM.nextInt(types.length)];
    }

    /**
     * 生成随机使用工具（枚举）
     */
    public static ToolTypeEnum generateToolType() {
        ToolTypeEnum[] tools = ToolTypeEnum.values();
        return tools[RANDOM.nextInt(tools.length)];
    }
}

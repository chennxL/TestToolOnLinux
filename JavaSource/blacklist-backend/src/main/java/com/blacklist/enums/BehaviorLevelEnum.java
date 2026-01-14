package com.blacklist.enums;

import com.baomidou.mybatisplus.annotation.EnumValue;
import com.fasterxml.jackson.annotation.JsonValue;
import lombok.Getter;
/**
 * 行为评级枚举
 */
@Getter
public enum BehaviorLevelEnum {

    LEVEL_A(1, "A"),
    LEVEL_B(2, "B"),
    LEVEL_C(3, "C");

    @EnumValue  // MyBatis Plus会将此字段值存入数据库
    private final Integer code;

    @JsonValue  // JSON序列化时使用description
    private final String description;

    BehaviorLevelEnum(Integer code, String description) {
        this.code = code;
        this.description = description;
    }

    /**
     * 根据code获取枚举
     */
    public static BehaviorLevelEnum getByCode(Integer code) {
        if (code == null) {
            return null;
        }
        for (BehaviorLevelEnum level : values()) {
            if (level.getCode().equals(code)) {
                return level;
            }
        }
        return null;
    }

    /**
     * 根据code获取描述
     */
    public static String getDescByCode(Integer code) {
        BehaviorLevelEnum level = getByCode(code);
        return level != null ? level.getDescription() : null;
    }

    /**
     * 随机获取一个评级（用于生成测试数据）
     */
    public static BehaviorLevelEnum random() {
        BehaviorLevelEnum[] values = values();
        int index = (int) (Math.random() * values.length);
        return values[index];
    }
}
package com.blacklist.enums;

import com.baomidou.mybatisplus.annotation.EnumValue;
import com.fasterxml.jackson.annotation.JsonValue;
import lombok.Getter;
/**
 * 行为类型枚举
 */
@Getter
public enum BehaviorTypeEnum {

    HIDE(1, "藏匿"),
    COMPLAIN(2, "投诉"),
    OTHER(3, "其他");

    @EnumValue  // MyBatis Plus会将此字段值存入数据库
    private final Integer code;

    @JsonValue  // JSON序列化时使用description
    private final String description;

    BehaviorTypeEnum(Integer code, String description) {
        this.code = code;
        this.description = description;
    }

    /**
     * 根据code获取枚举
     */
    public static BehaviorTypeEnum getByCode(Integer code) {
        if (code == null) {
            return null;
        }
        for (BehaviorTypeEnum type : values()) {
            if (type.getCode().equals(code)) {
                return type;
            }
        }
        return null;
    }

    /**
     * 根据code获取描述
     */
    public static String getDescByCode(Integer code) {
        BehaviorTypeEnum type = getByCode(code);
        return type != null ? type.getDescription() : null;
    }
}
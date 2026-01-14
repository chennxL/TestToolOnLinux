package com.blacklist.enums;
import com.baomidou.mybatisplus.annotation.EnumValue;
import com.fasterxml.jackson.annotation.JsonValue;
import lombok.Getter;
/**
 * 使用工具枚举
 */
@Getter
public enum ToolTypeEnum {

    KNIFE(1, "刀具"),
    LIGHTER(2, "打火机"),
    OTHER(3, "其他");

    @EnumValue  // MyBatis Plus会将此字段值存入数据库
    private final Integer code;

    @JsonValue  // JSON序列化时使用description
    private final String description;

    ToolTypeEnum(Integer code, String description) {
        this.code = code;
        this.description = description;
    }

    /**
     * 根据code获取枚举
     */
    public static ToolTypeEnum getByCode(Integer code) {
        if (code == null) {
            return null;
        }
        for (ToolTypeEnum type : values()) {
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
        ToolTypeEnum type = getByCode(code);
        return type != null ? type.getDescription() : null;
    }
}

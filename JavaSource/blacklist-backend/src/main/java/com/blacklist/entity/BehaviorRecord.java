package com.blacklist.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import com.blacklist.enums.BehaviorTypeEnum;
import com.blacklist.enums.ToolTypeEnum;
import lombok.Data;

import java.time.LocalDateTime;

/**
 * 行为记录表实体
 */
@Data
@TableName("behavior_record")
public class BehaviorRecord {

    /**
     * 行为记录唯一标识（主键，自增）
     */
    @TableId(type = IdType.AUTO)
    private Long recordId;

    /**
     * 关联黑名单主表的user_id（外键）
     */
    private Long userId;

    /**
     * 行为类型（枚举）
     */
    private BehaviorTypeEnum behaviorType;

    /**
     * 使用工具（枚举）
     */
    private ToolTypeEnum tool;

}

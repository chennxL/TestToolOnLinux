package com.blacklist.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import com.blacklist.enums.BehaviorLevelEnum;
import lombok.Data;

/**
 * 黑名单主表实体
 */
@Data
@TableName("blacklist_main")
public class BlacklistMain {

    /**
     * 用户唯一标识（主键，自增）
     */
    @TableId(type = IdType.AUTO)
    private Long userId;

    /**
     * 身份证号（最后一位固定为X）
     */
    private String idCard;

    /**
     * 行为评级（高风险、中风险、低风险）
     */
    private BehaviorLevelEnum riskLevel;

    /**
     * 行为记录数（1-3）
     */
    private Integer recordCount;
}

package com.blacklist.dto;
import com.blacklist.entity.BehaviorRecord;
import com.blacklist.entity.BlacklistMain;
import lombok.Data;

import java.util.List;

/**
 * 黑名单完整信息DTO（包含主表和关联的行为记录）
 */
@Data
public class BlacklistFullInfo {
    /**
     * 黑名单主表信息
     */
    private BlacklistMain main;

    /**
     * 关联的行为记录列表
     */
    private List<BehaviorRecord> records;
}
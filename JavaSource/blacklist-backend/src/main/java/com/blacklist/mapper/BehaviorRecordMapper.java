package com.blacklist.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.blacklist.entity.BehaviorRecord;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Update;

/**
 * 行为记录表Mapper
 */
@Mapper
public interface BehaviorRecordMapper extends BaseMapper<BehaviorRecord> {
/**
     * 清空行为记录表
     */
    @Update("TRUNCATE TABLE behavior_record")
    void truncateTable();
}

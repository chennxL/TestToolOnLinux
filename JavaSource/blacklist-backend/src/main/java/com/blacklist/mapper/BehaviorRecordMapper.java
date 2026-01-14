package com.blacklist.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.blacklist.entity.BehaviorRecord;
import org.apache.ibatis.annotations.Mapper;

/**
 * 行为记录表Mapper
 */
@Mapper
public interface BehaviorRecordMapper extends BaseMapper<BehaviorRecord> {
}

package com.blacklist.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.blacklist.entity.BlacklistMain;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Update;

/**
 * 黑名单主表Mapper
 */
@Mapper
public interface BlacklistMainMapper extends BaseMapper<BlacklistMain> {
/**
     * 清空表（使用TRUNCATE，速度快，不会锁等待）
     */
    @Update("TRUNCATE TABLE blacklist_main")
    void truncateTable();
}

package com.blacklist.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.blacklist.common.BusinessException;
import com.blacklist.dto.BlacklistStatusDTO;
import com.blacklist.entity.BehaviorRecord;
import com.blacklist.entity.BlacklistMain;
import com.blacklist.enums.BehaviorLevelEnum;
import com.blacklist.enums.BehaviorTypeEnum;
import com.blacklist.mapper.BehaviorRecordMapper;
import com.blacklist.mapper.BlacklistMainMapper;
import com.blacklist.service.BlacklistService;
import com.blacklist.util.BehaviorGenerator;
import com.blacklist.util.IdCardGenerator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Set;

/**
 * 黑名单Service实现类
 */
@Slf4j
@Service
public class BlacklistServiceImpl implements BlacklistService {

    @Autowired
    private BlacklistMainMapper blacklistMainMapper;

    @Autowired
    private BehaviorRecordMapper behaviorRecordMapper;

    /**
     * 创建黑名单
     * @param size 黑名单规模
     */
    @Override
    @Transactional(rollbackFor = Exception.class)
    public void createBlacklist(Long size) {
        if (size == null || size < 1 || size > 50000000) {
            throw new BusinessException(400, "黑名单规模必须在1-5000万之间");
        }

        try {
            log.info("开始创建黑名单，规模: {}", size);

            // 清空原有数据
            blacklistMainMapper.delete(null);
            behaviorRecordMapper.delete(null);
            log.info("已清空原有黑名单数据");

            // 批量插入（分批处理，每批1000条）
            int batchSize = 1000;
            long totalBatches = (size + batchSize - 1) / batchSize;

            for (int batch = 0; batch < totalBatches; batch++) {
                long currentBatchSize = Math.min(batchSize, size - batch * batchSize);

                // 生成当前批次的身份证号（去重）
                Set<String> idCards = IdCardGenerator.generateInsideIdCards(currentBatchSize);
                List<BlacklistMain> mainList = new ArrayList<>();
                List<BehaviorRecord> recordList = new ArrayList<>();

                // 构建黑名单主表数据
                for (String idCard : idCards) {
                    BlacklistMain main = new BlacklistMain();
                    main.setIdCard(idCard);
                    main.setRiskLevel(BehaviorLevelEnum.random());  // 随机生成评级
                    main.setRecordCount(generateRecordCount());      // 生成1-3的记录数
                    mainList.add(main);
                }

                // 批量插入主表
                blacklistMainMapper.insert(mainList);

                // 构建行为记录表数据
                for (BlacklistMain main : mainList) {
                    int recordCount = main.getRecordCount();
                    for (int i = 0; i < recordCount; i++) {
                        BehaviorRecord record = new BehaviorRecord();
                        record.setUserId(main.getUserId());
                        record.setBehaviorType(BehaviorGenerator.generateBehaviorType());
                        record.setTool(BehaviorGenerator.generateToolType());
                        recordList.add(record);
                    }
                }

                // 批量插入行为记录表
                if (!recordList.isEmpty()) {
                    behaviorRecordMapper.insert(recordList);
                }

                // 打印进度
                int progress = (int) ((batch + 1) * 100.0 / totalBatches);
                log.info("黑名单创建进度: {}%", progress);
            }

            log.info("黑名单创建完成，总规模: {}", size);

        } catch (Exception e) {
            log.error("黑名单创建失败", e);
            throw new BusinessException("黑名单创建失败: " + e.getMessage());
        }
    }

    /**
     * 生成随机记录数（1-3）
     */
    private int generateRecordCount() {
        return new Random().nextInt(3) + 1;
    }
    /**
     * 获取黑名单状态
     */
    @Override
    public BlacklistStatusDTO getStatus() {
        BlacklistStatusDTO dto = new BlacklistStatusDTO();
        
        // 查询黑名单总数
        Long count = blacklistMainMapper.selectCount(null);
        
        if (count > 0) {
            dto.setStatus("已创建");
            dto.setSize(count);
        } else {
            dto.setStatus("未创建");
            dto.setSize(0L);
        }
        
        return dto;
    }
    
    /**
     * 获取黑名单数量
     * @return 黑名单总数
     */
    @Override
    public Long getCount() {
        Long count = blacklistMainMapper.selectCount(null);
        log.debug("查询黑名单数量: {}", count);
        return count != null ? count : 0L;
    }
}

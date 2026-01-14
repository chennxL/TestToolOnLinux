package com.blacklist.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.blacklist.common.BusinessException;
import com.blacklist.dto.BlacklistFullInfo;
import com.blacklist.dto.QueryResultDTO;
import com.blacklist.entity.BehaviorRecord;
import com.blacklist.entity.BlacklistMain;
import com.blacklist.grpc.PSIGrpcClient;
import com.blacklist.mapper.BehaviorRecordMapper;
import com.blacklist.mapper.BlacklistMainMapper;
import com.blacklist.service.TestSetService;
import com.blacklist.util.IdCardGenerator;
import lombok.extern.slf4j.Slf4j;
import org.apache.poi.ss.usermodel.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.*;
import java.util.stream.Collectors;

/**
 * 测试集Service实现类
 */
@Slf4j
@Service
public class TestSetServiceImpl implements TestSetService {

    @Autowired
    private BlacklistMainMapper blacklistMainMapper;

    @Autowired
    private BehaviorRecordMapper behaviorRecordMapper;

    @Autowired
    private PSIGrpcClient psiGrpcClient;

    /**
     * 创建测试集
     */
    /**
     * 生成测试集明文数据（返回给Qt）
     */
    @Override
    public List<String> createTestSet(Integer insideSize, Integer outsideSize) {
        // 参数校验
        if (insideSize == null || insideSize < 0 || insideSize > 5000) {
            throw new BusinessException(400, "库内规模必须在0-5000之间");
        }
        if (outsideSize == null || outsideSize < 0 || outsideSize > 5000) {
            throw new BusinessException(400, "库外规模必须在0-5000之间");
        }
        if (insideSize + outsideSize < 1) {
            throw new BusinessException(400, "总规模必须大于0");
        }

        // 检查黑名单是否已创建
        Long blacklistCount = blacklistMainMapper.selectCount(null);
        if (blacklistCount == 0) {
            throw new BusinessException(400, "请先创建黑名单");
        }

        try {
            log.info("开始生成测试集明文，库内: {}, 库外: {}", insideSize, outsideSize);

            List<String> idCardList = new ArrayList<>();

            // 生成库内数据（从黑名单中随机抽取）
            if (insideSize > 0) {
                if (insideSize > blacklistCount) {
                    throw new BusinessException(400,
                            String.format("库内规模(%d)超过黑名单总数(%d)", insideSize, blacklistCount));
                }

                QueryWrapper<BlacklistMain> wrapper = new QueryWrapper<>();
                wrapper.last("ORDER BY RAND() LIMIT " + insideSize);
                List<BlacklistMain> insideList = blacklistMainMapper.selectList(wrapper);

                for (BlacklistMain main : insideList) {
                    idCardList.add(main.getIdCard());
                }

                log.info("库内数据生成完成: {} 条", insideSize);
            }

            // 生成库外数据（随机生成新身份证号）
            if (outsideSize > 0) {
                Set<String> outsideIdCards = IdCardGenerator.generateOutsideIdCards(outsideSize);
                idCardList.addAll(outsideIdCards);

                log.info("库外数据生成完成: {} 条", outsideSize);
            }

            log.info("测试集明文数据生成完成，总数: {}", idCardList.size());

            return idCardList;
        } catch (BusinessException e) {
            throw e;
        } catch (Exception e) {
            log.error("测试集生成失败", e);
            throw new BusinessException("测试集生成失败: " + e.getMessage());
        }
    }

    @Override
    @Transactional
    public void saveEncryptedData(String payloadData, String contextData) {
        log.info("开始保存加密数据");

        // 校验参数
        if (payloadData == null || payloadData.isEmpty()) {
            throw new BusinessException(400, "加密负载数据不能为空");
        }
        if (contextData == null || contextData.isEmpty()) {
            throw new BusinessException(400, "上下文数据不能为空");
        }
        log.info("payloadData:"+payloadData.length());
    }

    @Override
    public QueryResultDTO queryBlacklist(String payloadData, String contextData) {
        log.info("开始执行黑名单查询");
        long startTime = System.currentTimeMillis();

        try {
            // 1. 从数据库查询黑名单完整数据（主表 + 行为记录）
            log.info("查询黑名单完整数据...");
            List<BlacklistFullInfo> blacklistFullData = queryAllBlacklistWithRecords();

            if (blacklistFullData.isEmpty()) {
                throw new BusinessException(400, "黑名单库为空");
            }

            log.info("黑名单数据量: {}", blacklistFullData.size());

            // 2. 调用gRPC进行PSI匹配（传递完整数据）
            log.info("调用gRPC进行PSI匹配...");
            String encryptedResult = psiGrpcClient.doMatch(contextData, payloadData, blacklistFullData);

            // 3. 解析匹配数量（需要根据C++服务器返回的格式来解析）
            // 暂时返回0，后续需要实现解析逻辑
            int matchCount = parseMatchCount(encryptedResult);

            long endTime = System.currentTimeMillis();
            log.info("查询完成，耗时: {}ms, 匹配数: {}", endTime - startTime, matchCount);

            // 4. 构建返回结果
            QueryResultDTO result = new QueryResultDTO();
            result.setEncryptedResult(encryptedResult);  // 返回给Qt用于解密
            result.setMatchCount(matchCount);
            result.setTotalCount(blacklistFullData.size());

            return result;

        } catch (BusinessException e) {
            throw e;
        } catch (Exception e) {
            log.error("查询失败", e);
            throw new BusinessException("查询失败: " + e.getMessage());
        }
    }

    /**
     * 查询所有黑名单完整信息（包含行为记录）
     */
    private List<BlacklistFullInfo> queryAllBlacklistWithRecords() {
        log.info("开始查询黑名单完整数据（主表+行为记录）");

        // 1. 查询所有黑名单主表
        List<BlacklistMain> mainList = blacklistMainMapper.selectList(null);
        log.info("查询到黑名单主表数据: {} 条", mainList.size());

        if (mainList.isEmpty()) {
            return new ArrayList<>();
        }

        // 2. 提取所有 userId
        List<Long> userIds = mainList.stream()
                .map(BlacklistMain::getUserId)
                .collect(Collectors.toList());

        // 3. 批量查询所有行为记录
        List<BehaviorRecord> allRecords = behaviorRecordMapper.selectList(
                new LambdaQueryWrapper<BehaviorRecord>()
                        .in(BehaviorRecord::getUserId, userIds)
        );
        log.info("查询到行为记录数据: {} 条", allRecords.size());

        // 4. 按 userId 分组行为记录
        Map<Long, List<BehaviorRecord>> recordMap = allRecords.stream()
                .collect(Collectors.groupingBy(BehaviorRecord::getUserId));

        // 5. 组装完整信息
        List<BlacklistFullInfo> result = new ArrayList<>();
        for (BlacklistMain main : mainList) {
            BlacklistFullInfo info = new BlacklistFullInfo();
            info.setMain(main);
            info.setRecords(recordMap.getOrDefault(main.getUserId(), new ArrayList<>()));
            result.add(info);
        }

        log.info("组装完成，共 {} 条完整黑名单信息", result.size());
        return result;
    }

    /**
     * 解析匹配数量（暂时返回0，后续根据C++返回格式实现）
     */
    private int parseMatchCount(String encryptedResult) {
        // TODO: 根据C++服务器返回的加密结果格式，解析出匹配数量
        // 这里需要和C++同事确认返回格式
        return 0;
    }
}

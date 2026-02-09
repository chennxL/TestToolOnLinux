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
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.baomidou.mybatisplus.core.metadata.IPage;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
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

            // 2. 生成库外数据（确保前17位不与黑名单碰撞）
        if (outsideSize > 0) {
            log.info("开始生成库外数据，需避免前17位与黑名单碰撞...");
            
            // 2.1 获取黑名单所有前17位（用于碰撞检测）
            Set<String> blacklistPrefix17 = new HashSet<>();
            
            // 分批查询避免内存溢出
            long batchSize = 10000;
            long totalBatches = (blacklistCount + batchSize - 1) / batchSize;
            
            for (long i = 0; i < totalBatches; i++) {
                QueryWrapper<BlacklistMain> batchWrapper = new QueryWrapper<>();
                batchWrapper.select("id_card")
                           .last("LIMIT " + (i * batchSize) + ", " + batchSize);
                List<BlacklistMain> batch = blacklistMainMapper.selectList(batchWrapper);
                
                for (BlacklistMain main : batch) {
                    blacklistPrefix17.add(main.getIdCard().substring(0, 17));
                }
                
                if ((i + 1) % 10 == 0 || i == totalBatches - 1) {
                    log.info("黑名单前17位加载进度: {}/{} 批次", i + 1, totalBatches);
                }
            }
            
            log.info("黑名单前17位唯一数量: {} (总记录: {})", 
                    blacklistPrefix17.size(), blacklistCount);
            
            // 2.2 生成库外数据，跳过前17位碰撞
            Set<String> outsideIdCards = new HashSet<>();
            int attempts = 0;
            int maxAttempts = outsideSize * 1000; // 最大尝试次数
            int skippedCollisions = 0; // 跳过的碰撞数
            int skippedDuplicates = 0; // 跳过的自身重复数
            
            while (outsideIdCards.size() < outsideSize && attempts < maxAttempts) {
                String newId = IdCardGenerator.generateOutsideIdCard();
                String prefix17 = newId.substring(0, 17);
                
                attempts++;
                
                // 检查1：前17位不能在黑名单中
                if (blacklistPrefix17.contains(prefix17)) {
                    skippedCollisions++;
                    continue;
                }
                
                // 检查2：库外数据内部不重复
                if (!outsideIdCards.add(newId)) {
                    skippedDuplicates++;
                    continue;
                }
                
                // 进度提示
                if (outsideIdCards.size() % 1000 == 0) {
                    log.info("库外数据生成进度: {}/{} (已尝试: {}, 跳过碰撞: {}, 跳过重复: {})", 
                            outsideIdCards.size(), outsideSize, attempts, 
                            skippedCollisions, skippedDuplicates);
                }
            }
            
            // 检查是否生成成功
            if (outsideIdCards.size() < outsideSize) {
                log.error("库外数据生成不足！目标: {}, 实际: {}, 尝试次数: {}", 
                        outsideSize, outsideIdCards.size(), attempts);
                log.error("跳过碰撞: {}, 跳过重复: {}", skippedCollisions, skippedDuplicates);
                throw new BusinessException(500, 
                        String.format("库外数据生成失败：仅生成 %d/%d 条，黑名单规模过大导致碰撞率过高", 
                                outsideIdCards.size(), outsideSize));
            }
            
            idCardList.addAll(outsideIdCards);
            log.info("库外数据生成完成: {} 条（跳过 {} 个前17位碰撞，{} 个自身重复）", 
                    outsideIdCards.size(), skippedCollisions, skippedDuplicates);
        }
        
        log.info("测试集明文数据生成完成，总数: {}", idCardList.size());
        
        // 导出测试集到文件
        try {
            String exportPath = exportTestSetToFile(idCardList, insideSize, outsideSize);
            log.info("测试集已导出到: {}", exportPath);
        } catch (IOException e) {
            log.error("测试集导出失败", e);
            // 导出失败不影响返回结果
        }
        
        return idCardList;
        
        } catch (BusinessException e) {
            throw e;
        } catch (Exception e) {
            log.error("测试集生成失败", e);
            throw new BusinessException("测试集生成失败: " + e.getMessage());
        }
    }

    /**
     * 导出测试集到文件（仅身份证号）
     */
    private String exportTestSetToFile(List<String> idCardList, int insideSize, int outsideSize) throws IOException {
        // 导出目录
        String exportDir = "./testset_exports";
        File dir = new File(exportDir);
        if (!dir.exists()) {
            dir.mkdirs();
        }

        // 生成文件名：testset_库内数量_库外数量_时间戳.txt
        SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String timestamp = sdf.format(new Date());
        String fileName = String.format("testset_%d_inside_%d_outside_%s.txt", 
                                       insideSize, outsideSize, timestamp);
        File exportFile = new File(dir, fileName);

        log.info("开始导出测试集到: {}", exportFile.getAbsolutePath());

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(exportFile))) {
            // 写入每个身份证号，每行一个
            for (String idCard : idCardList) {
                writer.write(idCard);
                writer.newLine();
            }
            writer.flush();
        }

        log.info("测试集导出成功，共 {} 条身份证号", idCardList.size());
        return exportFile.getAbsolutePath();
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
    public QueryResultDTO queryBlacklist(String payloadData, String contextData ,Integer weight, Integer effectiveLambda, Integer logPolyMod) {
        log.info("开始执行黑名单查询");
        log.info("PSI参数 - weight: {}, effectiveLambda: {}, logPolyMod: {}",
                weight, effectiveLambda, logPolyMod);
        long startTime = System.currentTimeMillis();

        try {
            // 1. 从数据库查询黑名单完整数据（主表 + 行为记录）
            log.info("查询黑名单完整数据...");
            long step1Start = System.currentTimeMillis();
            List<BlacklistFullInfo> blacklistFullData = queryAllBlacklistWithRecords();
            long step1Time = System.currentTimeMillis() - step1Start;
            log.info("步骤1-查询数据库耗时: {}ms", step1Time);

            if (blacklistFullData.isEmpty()) {
                throw new BusinessException(400, "黑名单库为空");
            }

            log.info("黑名单数据量: {}", blacklistFullData.size());

            // 2. 调用gRPC进行PSI匹配（传递完整数据）
            log.info("调用gRPC进行PSI匹配...");
            long step2Start = System.currentTimeMillis();
            String encryptedResult = psiGrpcClient.doMatch(contextData, payloadData, blacklistFullData,
                weight,           // 新增
                effectiveLambda,  // 新增
                logPolyMod        // 新增
                );
            long step2Time = System.currentTimeMillis() - step2Start;
            log.info("步骤2-gRPC调用耗时: {}ms", step2Time);

            // 3. 解析匹配数量（需要根据C++服务器返回的格式来解析）
            // 暂时返回0，后续需要实现解析逻辑
            int matchCount = parseMatchCount(encryptedResult);

            long endTime = System.currentTimeMillis();
            log.info("查询完成，耗时: {}ms, 匹配数: {}", endTime - startTime, matchCount);
            
            // ✅ 关键：立即释放大对象的引用
            int dataSize = blacklistFullData.size();
            blacklistFullData.clear();  // 清空List
            blacklistFullData = null;   // 释放引用

            // 4. 构建返回结果
            QueryResultDTO result = new QueryResultDTO();
            result.setEncryptedResult(encryptedResult);  // 返回给Qt用于解密
            result.setMatchCount(matchCount);
            result.setTotalCount(dataSize);
            
            // ✅ 主动触发GC（临时方案）
            System.gc();

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
    /**
 * 查询所有黑名单完整信息（包含行为记录）
 * 修改：使用分页查询避免一次性加载大量数据导致OOM
 */
private List<BlacklistFullInfo> queryAllBlacklistWithRecords() {
    log.info("开始查询黑名单完整数据（主表+行为记录）");

    List<BlacklistFullInfo> result = new ArrayList<>();
    int pageSize = 1000; // 每次查询1000条
    int currentPage = 1;
    
    while (true) {
        log.info("查询第 {} 页黑名单数据...", currentPage);
        
        // 【修改1】使用分页查询主表，避免一次性加载所有数据
        Page<BlacklistMain> page = new Page<>(currentPage, pageSize);
        IPage<BlacklistMain> pageResult = blacklistMainMapper.selectPage(page, null);
        List<BlacklistMain> mainList = pageResult.getRecords();
        
        if (mainList.isEmpty()) {
            break;
        }
        
   //     log.info("第 {} 页查询到 {} 条主表数据", currentPage, mainList.size());
        
        // 2. 提取这一批的 userId
        List<Long> userIds = mainList.stream()
                .map(BlacklistMain::getUserId)
                .collect(Collectors.toList());
        
        // 3. 批量查询这一批的行为记录
        List<BehaviorRecord> records = behaviorRecordMapper.selectList(
                new LambdaQueryWrapper<BehaviorRecord>()
                        .in(BehaviorRecord::getUserId, userIds)
        );
        
        // 4. 按 userId 分组行为记录
        Map<Long, List<BehaviorRecord>> recordMap = records.stream()
                .collect(Collectors.groupingBy(BehaviorRecord::getUserId));
        
        // 5. 组装这一批的完整信息
        for (BlacklistMain main : mainList) {
            BlacklistFullInfo info = new BlacklistFullInfo();
            info.setMain(main);
            info.setRecords(recordMap.getOrDefault(main.getUserId(), new ArrayList<>()));
            result.add(info);
        }
        
        log.info("第 {} 页数据处理完成，当前累计: {} 条", currentPage, result.size());
        
        // 【修改2】判断是否还有下一页
        if (currentPage >= pageResult.getPages()) {
            log.info("已查询完所有页，总页数: {}", pageResult.getPages());
            break;
        }
        
        currentPage++;
    }
    
    log.info("分页查询完成，共 {} 条完整黑名单信息", result.size());
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

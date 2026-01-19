package com.blacklist.util;

import lombok.extern.slf4j.Slf4j;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;

/**
 * 身份证号哈希工具类
 * 使用与Qt端相同的SHA256算法
 */
@Slf4j
public class IdCardHashUtil {

    /**
     * 将身份证号转换为size_t(long)
     * 使用SHA256哈希，取前8字节
     *
     * 注意：必须与Qt端的 CryptoWrapper::hashIdCard() 保持一致
     */
    public static long hashIdCard(String idCard) {
        if (idCard == null || idCard.isEmpty()) {
            return 0L;
        }

        try {
            // 使用SHA256哈希
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hash = digest.digest(idCard.getBytes(StandardCharsets.UTF_8));

            // 取前8字节转换为long（size_t）
            long result = 0L;
            for (int i = 0; i < 8 && i < hash.length; i++) {
                result = (result << 8) | (hash[i] & 0xFF);
            }

            return result;

        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("SHA-256算法不可用", e);
        }
    }

    /**
     * 批量转换身份证号列表
     */
/**
     * 批量转换身份证号列表（带冲突检测）
     */
    public static Map<Long, String> batchHashIdCards(List<String> idCards) {
        if (idCards == null || idCards.isEmpty()) {
            log.warn("输入的身份证号列表为空");
            return new HashMap<>();
        }
        
        log.info("========== 开始批量哈希身份证号 ==========");
        log.info("输入身份证号数量: {}", idCards.size());
        
        Map<Long, String> hashMap = new HashMap<>();
        Map<Long, List<String>> collisionMap = new HashMap<>();  // 用于检测冲突
        
        // 处理每个身份证号
        for (String idCard : idCards) {
            if (idCard == null || idCard.isEmpty()) {
                log.warn("发现空身份证号，跳过");
                continue;
            }
            
            long hash = hashIdCard(idCard);
            
            // 检测冲突
            if (hashMap.containsKey(hash)) {
                String existingIdCard = hashMap.get(hash);
                
                // 如果是不同的身份证号，说明发生了哈希冲突
                if (!existingIdCard.equals(idCard)) {
                    // 记录冲突信息
                    collisionMap.computeIfAbsent(hash, k -> new ArrayList<>());
                    if (!collisionMap.get(hash).contains(existingIdCard)) {
                        collisionMap.get(hash).add(existingIdCard);
                    }
                    collisionMap.get(hash).add(idCard);
                }
            } else {
                hashMap.put(hash, idCard);
            }
        }
        
        log.info("---------- 批量哈希统计 ----------");
        log.info("总输入: {} 条", idCards.size());
        log.info("唯一哈希值: {} 个", hashMap.size());
        log.info("发现冲突: {} 个哈希值", collisionMap.size());
        
        // 如果有冲突，打印详细信息
        if (!collisionMap.isEmpty()) {
            log.error("========== ⚠️  哈希冲突详情 ==========");
            log.error("共发现 {} 个冲突的哈希值", collisionMap.size());
            
            int conflictIndex = 0;
            for (Map.Entry<Long, List<String>> entry : collisionMap.entrySet()) {
                conflictIndex++;
                long conflictHash = entry.getKey();
                List<String> conflictIdCards = entry.getValue();
                
                log.error("冲突 #{}: 哈希值 {} (0x{}) 对应 {} 个身份证号:", 
                    conflictIndex, 
                    conflictHash, 
                    Long.toHexString(conflictHash), 
                    conflictIdCards.size());
                
                for (int i = 0; i < conflictIdCards.size(); i++) {
                    String idCard = conflictIdCards.get(i);
                    // 脱敏显示：只显示前6位和后4位
                    String masked = idCard.length() > 10 ? 
                        idCard.substring(0, 6) + "****" + idCard.substring(idCard.length() - 4) : 
                        idCard;
                    log.error("  [{}] {}", i + 1, masked);
                }
            }
            log.error("====================================");
        } else {
            log.info("✓ 未发现哈希冲突");
        }
        
        log.info("======================================");
        
        return hashMap;
    }

}

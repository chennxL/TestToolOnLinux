package com.blacklist.util;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * 身份证号哈希工具类
 * 使用与Qt端相同的SHA256算法
 */
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
    public static java.util.Map<Long, String> batchHashIdCards(java.util.List<String> idCards) {
        java.util.Map<Long, String> hashMap = new java.util.HashMap<>();
        for (String idCard : idCards) {
            long hash = hashIdCard(idCard);
            hashMap.put(hash, idCard);
        }
        return hashMap;
    }
}
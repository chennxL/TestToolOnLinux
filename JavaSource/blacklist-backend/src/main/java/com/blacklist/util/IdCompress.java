package com.blacklist.util;

import lombok.extern.slf4j.Slf4j;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 身份证号压缩/解压工具类
 * 与C++端 id_compress.cc 保持完全一致
 * 使用无碰撞的压缩编码替代哈希
 */
@Slf4j
public class IdCompress {
    
    // 支持的地区码（与C++完全一致）
    private static final int[] REGIONS = {
        11, 12, 13, 14, 15, 21, 22, 23, 31, 32, 33, 34, 35, 36, 37, 41, 42,
        43, 44, 45, 46, 50, 51, 52, 53, 54, 61, 62, 63, 64, 65, 71, 81, 82
    };
    
    private static final Map<Integer, Integer> REGION_MAP = new HashMap<>();
    static {
        for (int i = 0; i < REGIONS.length; i++) {
            REGION_MAP.put(REGIONS[i], i);
        }
    }
    
    // 校验码权重
    private static final int[] WEIGHTS = {
        7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2
    };
    
    // 校验码字符
    private static final char[] CHECK_DIGITS = {
        '1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'
    };
    
    /**
     * 压缩身份证号为64位整数
     * 
     * 编码格式（46位有效）：
     * bits 45-27: 地区索引(6位) + 区域码(13位) = 19位
     * bits 26-19: 年份偏移(1800-2055) = 8位
     * bits 18-15: 月份(1-12) = 4位  
     * bits 14-10: 日期(1-31) = 5位
     * bits  9-0:  序列号(0-999) = 10位
     * 
     * @param id 18位身份证号
     * @return 压缩后的64位整数
     */
    public static long compress(String id) {
        if (id == null || id.length() < 17) {
            throw new IllegalArgumentException("Invalid ID card length: " + 
                (id == null ? "null" : id.length()));
        }
        
        // 只取前17位（忽略校验码）
        String id17 = id.substring(0, 17);
        
        // 验证是否全为数字
        for (char c : id17.toCharArray()) {
            if (c < '0' || c > '9') {
                throw new IllegalArgumentException("ID contains non-digit characters: " + id);
            }
        }
        
        // 解析各部分
        int region = Integer.parseInt(id.substring(0, 2));
        int area = Integer.parseInt(id.substring(2, 6));
        int year = Integer.parseInt(id.substring(6, 10));
        int month = Integer.parseInt(id.substring(10, 12));
        int day = Integer.parseInt(id.substring(12, 14));
        int seq = Integer.parseInt(id.substring(14, 17));
        
        // 验证范围
        if (!REGION_MAP.containsKey(region)) {
            throw new IllegalArgumentException("Invalid region code: " + region);
        }
        if (year < 1800 || year > 2055) {
            throw new IllegalArgumentException("Invalid year: " + year);
        }
        if (month < 1 || month > 12) {
            throw new IllegalArgumentException("Invalid month: " + month);
        }
        if (day < 1 || day > 31) {
            throw new IllegalArgumentException("Invalid day: " + day);
        }
        
        // 编码
        long regionIdx = REGION_MAP.get(region);
        long yearOffset = year - 1800;
        long regionArea = regionIdx * 10000 + area;
        
        long c = 0;
        c |= (regionArea & 0x7FFFFL) << 27;  // 19位
        c |= (yearOffset & 0xFFL) << 19;     // 8位
        c |= (month & 0x0FL) << 15;          // 4位
        c |= (day & 0x1FL) << 10;            // 5位
        c |= (seq & 0x3FFL);                 // 10位
        
        return c;
    }
    
    /**
     * 解压64位整数为身份证号
     * 
     * @param compressed 压缩的64位整数
     * @return 18位身份证号（含校验码）
     */
    public static String decompress(long compressed) {
        // 解码各部分
        int regionArea = (int)((compressed >> 27) & 0x7FFFFL);
        int yearOffset = (int)((compressed >> 19) & 0xFFL);
        int month = (int)((compressed >> 15) & 0x0FL);
        int day = (int)((compressed >> 10) & 0x1FL);
        int seq = (int)(compressed & 0x3FFL);
        
        int regionIdx = regionArea / 10000;
        int area = regionArea % 10000;
        
        if (regionIdx >= REGIONS.length) {
            throw new IllegalArgumentException("Invalid compressed data: region index=" + regionIdx);
        }
        
        int region = REGIONS[regionIdx];
        int year = yearOffset + 1800;
        
        // 拼接身份证号前17位
        String id17 = String.format("%02d%04d%04d%02d%02d%03d",
            region, area, year, month, day, seq);
        
        // 计算并添加校验码
        return id17 + calcCheckDigit(id17);
    }
    
    /**
     * 计算校验码
     */
    private static char calcCheckDigit(String id17) {
        int sum = 0;
        for (int i = 0; i < 17; i++) {
            sum += (id17.charAt(i) - '0') * WEIGHTS[i];
        }
        return CHECK_DIGITS[sum % 11];
    }
    
    /**
     * 批量压缩身份证号
     * 
     * @param idCards 身份证号列表
     * @return Map<压缩后的ID, 原始身份证号>
     */
    public static Map<Long, String> batchCompress(List<String> idCards) {
        Map<Long, String> compressedMap = new HashMap<>();
        
        for (String idCard : idCards) {
            try {
                long compressed = compress(idCard);
                compressedMap.put(compressed, idCard);
            } catch (Exception e) {
                log.error("压缩身份证号失败: {}", idCard, e);
                // 继续处理其他身份证号
            }
        }
        
        return compressedMap;
    }
}

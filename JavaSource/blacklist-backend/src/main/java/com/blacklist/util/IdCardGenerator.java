package com.blacklist.util;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.*;

/**
 * 身份证号生成工具类
 */
public class IdCardGenerator {

    private static final Random RANDOM = new Random();
    
    // 行政区划代码（前6位）- 示例
    private static final String[] AREA_CODES = {
        "110101", // 北京东城
        "110102", // 北京西城
        "310101", // 上海黄浦
        "310104", // 上海徐汇
        "440103", // 广州荔湾
        "440104", // 广州越秀
        "500101", // 重庆万州
        "320102", // 南京玄武
        "330102", // 杭州上城
        "420102"  // 武汉江岸
    };

    /**
     * 生成库内身份证号（最后一位固定为X）
     * @return 18位身份证号
     */
    public static String generateInsideIdCard() {
        StringBuilder idCard = new StringBuilder();
        
        // 1. 行政区划代码（6位）
        String areaCode = AREA_CODES[RANDOM.nextInt(AREA_CODES.length)];
        idCard.append(areaCode);
        
        // 2. 出生日期（8位）YYYYMMDD
        String birthDate = generateBirthDate();
        idCard.append(birthDate);
        
        // 3. 顺序码（3位）
        int sequence = RANDOM.nextInt(999) + 1;
        idCard.append(String.format("%03d", sequence));
        
        // 4. 最后一位固定为X（用于标识库内数据）
        idCard.append("X");
        
        return idCard.toString();
    }

    /**
     * 生成库外身份证号（最后一位不为X）
     * @return 18位身份证号
     */
    public static String generateOutsideIdCard() {
        StringBuilder idCard = new StringBuilder();
        
        // 前17位生成方式同库内
        String areaCode = AREA_CODES[RANDOM.nextInt(AREA_CODES.length)];
        idCard.append(areaCode);
        
        String birthDate = generateBirthDate();
        idCard.append(birthDate);
        
        int sequence = RANDOM.nextInt(999) + 1;
        idCard.append(String.format("%03d", sequence));
        
        // 最后一位：0-9随机数字（不能是X）
        int checkCode = RANDOM.nextInt(10);
        idCard.append(checkCode);
        
        return idCard.toString();
    }

    /**
     * 生成出生日期（YYYYMMDD格式）
     * 年龄范围：18-60岁
     */
    private static String generateBirthDate() {
        int year = LocalDate.now().getYear() - (18 + RANDOM.nextInt(43)); // 18-60岁
        int month = 1 + RANDOM.nextInt(12);
        int day = 1 + RANDOM.nextInt(28); // 简化处理，统一28天
        
        return String.format("%04d%02d%02d", year, month, day);
    }

    /**
     * 批量生成库内身份证号
     * @param count 生成数量
     * @return 身份证号集合（去重）
     */
    public static Set<String> generateInsideIdCards(long count) {
        Set<String> idCards = new HashSet<>();
        while (idCards.size() < count) {
            idCards.add(generateInsideIdCard());
        }
        return idCards;
    }

    /**
     * 批量生成库外身份证号
     * @param count 生成数量
     * @return 身份证号集合（去重）
     */
    public static Set<String> generateOutsideIdCards(int count) {
        Set<String> idCards = new HashSet<>();
        while (idCards.size() < count) {
            idCards.add(generateOutsideIdCard());
        }
        return idCards;
    }

    /**
     * 生成中文姓名
     */
    public static String generateName() {
        String[] surnames = {"张", "王", "李", "赵", "刘", "陈", "杨", "黄", "周", "吴"};
        String[] names = {"伟", "芳", "娜", "秀英", "敏", "静", "丽", "强", "磊", "军", "洋", "勇"};
        
        String surname = surnames[RANDOM.nextInt(surnames.length)];
        String name = names[RANDOM.nextInt(names.length)];
        
        // 20%概率生成两个字的名字
        if (RANDOM.nextDouble() < 0.2) {
            name += names[RANDOM.nextInt(names.length)];
        }
        
        return surname + name;
    }

    /**
     * 生成风险等级
     */
    public static String generateRiskLevel() {
        String[] levels = {"高风险", "中风险", "低风险"};
        // 按比例：高20%、中30%、低50%
        double rand = RANDOM.nextDouble();
        if (rand < 0.2) {
            return levels[0];
        } else if (rand < 0.5) {
            return levels[1];
        } else {
            return levels[2];
        }
    }

    /**
     * 生成行为记录数（1-3）
     */
    public static int generateRecordCount() {
        return 1 + RANDOM.nextInt(3);
    }
}

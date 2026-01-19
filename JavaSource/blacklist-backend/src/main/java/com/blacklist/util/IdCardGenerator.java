package com.blacklist.util;

import java.time.LocalDate;
import java.util.*;

/**
 * 身份证号生成工具类（优化版 - 避免碰撞）
 * 使用随机6位数行政区划码，大幅降低碰撞概率
 */
public class IdCardGenerator {

    private static final Random RANDOM = new Random();
    
    // 行政区划代码（前6位）- 覆盖全国主要省市区县
    private static final String[] AREA_CODES = {
    // 北京市 (11xxxx)
    "110101", // 东城区
    "110102", // 西城区
    "110105", // 朝阳区
    "110106", // 丰台区
    "110107", // 石景山区
    "110108", // 海淀区
    "110109", // 门头沟区
    "110111", // 房山区
    "110112", // 通州区
    "110113", // 顺义区
    
    // 天津市 (12xxxx)
    "120101", // 和平区
    "120102", // 河东区
    "120103", // 河西区
    "120104", // 南开区
    "120105", // 河北区
    "120106", // 红桥区
    
    // 河北省 (13xxxx)
    "130102", // 石家庄长安区
    "130104", // 石家庄桥西区
    "130202", // 唐山路南区
    "130302", // 秦皇岛海港区
    
    // 上海市 (31xxxx)
    "310101", // 黄浦区
    "310104", // 徐汇区
    "310105", // 长宁区
    "310106", // 静安区
    "310107", // 普陀区
    "310109", // 虹口区
    "310110", // 杨浦区
    "310112", // 闵行区
    "310113", // 宝山区
    "310114", // 嘉定区
    "310115", // 浦东新区
    
    // 江苏省 (32xxxx)
    "320102", // 南京玄武区
    "320104", // 南京秦淮区
    "320105", // 南京建邺区
    "320106", // 南京鼓楼区
    "320205", // 无锡锡山区
    "320505", // 苏州虎丘区
    "320506", // 苏州吴中区
    
    // 浙江省 (33xxxx)
    "330102", // 杭州上城区
    "330103", // 杭州下城区
    "330104", // 杭州江干区
    "330105", // 杭州拱墅区
    "330106", // 杭州西湖区
    "330203", // 宁波海曙区
    "330302", // 温州鹿城区
    
    // 广东省 (44xxxx)
    "440103", // 广州荔湾区
    "440104", // 广州越秀区
    "440105", // 广州海珠区
    "440106", // 广州天河区
    "440111", // 广州白云区
    "440303", // 深圳罗湖区
    "440304", // 深圳福田区
    "440305", // 深圳南山区
    "440306", // 深圳宝安区
    
    // 四川省 (51xxxx)
    "510104", // 成都锦江区
    "510105", // 成都青羊区
    "510106", // 成都金牛区
    "510107", // 成都武侯区
    "510108", // 成都成华区
    
    // 重庆市 (50xxxx)
    "500101", // 万州区
    "500102", // 涪陵区
    "500103", // 渝中区
    "500104", // 大渡口区
    "500105", // 江北区
    "500106",  // 沙坪坝区
    
    // 湖北省 (42xxxx)
    "420102", // 武汉江岸区
    "420103", // 武汉江汉区
    "420104", // 武汉硚口区
    "420105", // 武汉汉阳区
    
    // 湖南省 (43xxxx)
    "430102", // 长沙芙蓉区
    "430103", // 长沙天心区
    "430104", // 长沙岳麓区
    
    // 陕西省 (61xxxx)
    "610102", // 西安新城区
    "610103", // 西安碑林区
    "610104", // 西安莲湖区
    
    // 山东省 (37xxxx)
    "370102", // 济南历下区
    "370103", // 济南市中区
    "370202", // 青岛市南区
    
    // 辽宁省 (21xxxx)
    "210102", // 沈阳和平区
    "210103", // 沈阳沉河区
    "210202", // 大连中山区
    
    // 吉林省 (22xxxx)
    "220102", // 长春南关区
    "220103", // 长春宽城区
    
    // 黑龙江省 (23xxxx)
    "230102", // 哈尔滨道里区
    "230103", // 哈尔滨南岗区
    
    // 安徽省 (34xxxx)
    "340102", // 合肥瑶海区
    "340103", // 合肥庐阳区
    
    // 福建省 (35xxxx)
    "350102", // 福州鼓楼区
    "350203", // 厦门思明区
    
    // 江西省 (36xxxx)
    "360102", // 南昌东湖区
    "360103", // 南昌西湖区
    
    // 河南省 (41xxxx)
    "410102", // 郑州中原区
    "410103", // 郑州二七区
    
    // 山西省 (14xxxx)
    "140105", // 太原小店区
    "140106", // 太原迎泽区
    
    // 内蒙古 (15xxxx)
    "150102", // 呼和浩特新城区
    "150103", // 呼和浩特回民区
    
    // 广西 (45xxxx)
    "450102", // 南宁兴宁区
    "450103", // 南宁青秀区
    
    // 云南省 (53xxxx)
    "530102", // 昆明五华区
    "530103", // 昆明盘龙区
    
    // 贵州省 (52xxxx)
    "520102", // 贵阳南明区
    "520103"  // 贵阳云岩区
};
    
    /**
     * 生成库内身份证号（最后一位固定为X）
     * @return 18位身份证号
     */
    public static String generateInsideIdCard() {
        StringBuilder idCard = new StringBuilder();
        
        // 1. 行政区划代码（6位）- 使用随机6位数 (100000-999999)
        // 1. 行政区划（增加到50+个）
        String areaCode = AREA_CODES[RANDOM.nextInt(AREA_CODES.length)];
        idCard.append(areaCode);
        
        // 2. 出生日期（8位）YYYYMMDD - 扩大年龄范围
        String birthDate = generateBirthDate();
        idCard.append(birthDate);
        
        // 3. 顺序码（3位）- 使用完整的000-999
        int sequence = RANDOM.nextInt(1000);
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
        
        int sequence = RANDOM.nextInt(1000);
        idCard.append(String.format("%03d", sequence));
        
        // 最后一位：0-9随机数字（不能是X）
        int checkCode = RANDOM.nextInt(10);
        idCard.append(checkCode);
        
        return idCard.toString();
    }

    /**
     * 生成出生日期（YYYYMMDD格式）
     * 年龄范围：18-80岁（扩大范围降低碰撞）
     */
    private static String generateBirthDate() {
        int year = LocalDate.now().getYear() - (18 + RANDOM.nextInt(63)); // 18-80岁
        int month = 1 + RANDOM.nextInt(12);
        int day = 1 + RANDOM.nextInt(28); // 简化处理，统一28天
        
        return String.format("%04d%02d%02d", year, month, day);
    }

    /**
     * 批量生成库内身份证号（带进度监控，防止死循环）
     * @param count 生成数量
     * @return 身份证号集合（保证唯一）
     */
    public static Set<String> generateInsideIdCards(long count) {
        Set<String> idCards = new HashSet<>();
        
        long startTime = System.currentTimeMillis();
        int consecutiveFailures = 0;  // 连续失败计数
        int maxConsecutiveFailures = 10000;  // 最大连续失败次数
        
        System.out.println("开始生成库内身份证号，目标数量: " + count);
        
        while (idCards.size() < count) {
            String newId = generateInsideIdCard();
            boolean added = idCards.add(newId);
            
            if (!added) {
                consecutiveFailures++;
                if (consecutiveFailures >= maxConsecutiveFailures) {
                    System.err.println("警告：连续 " + maxConsecutiveFailures + " 次生成重复，可能需要增加随机空间！");
                    System.err.println("已生成：" + idCards.size() + " / " + count);
                    break;
                }
            } else {
                consecutiveFailures = 0;  // 重置失败计数
            }
            
            // 每1万条打印进度
            if (idCards.size() % 10000 == 0) {
                long elapsed = System.currentTimeMillis() - startTime;
                double progress = idCards.size() * 100.0 / count;
                System.out.println(String.format("生成进度: %d / %d (%.2f%%) - 耗时: %.2f 秒", 
                    idCards.size(), count, 
                    progress,
                    elapsed / 1000.0));
            }
        }
        
        long totalTime = System.currentTimeMillis() - startTime;
        System.out.println("========================================");
        System.out.println("生成完成！");
        System.out.println("  目标数量: " + count);
        System.out.println("  实际生成: " + idCards.size());
        System.out.println("  总耗时: " + (totalTime / 1000.0) + " 秒");
        System.out.println("  平均速度: " + (idCards.size() * 1000 / totalTime) + " 条/秒");
        System.out.println("========================================");
        
        return idCards;
    }

    /**
     * 批量生成库外身份证号
     * @param count 生成数量
     * @return 身份证号集合（去重）
     */
    public static Set<String> generateOutsideIdCards(int count) {
        Set<String> idCards = new HashSet<>();
        
        long startTime = System.currentTimeMillis();
        int consecutiveFailures = 0;
        int maxConsecutiveFailures = 10000;
        
        System.out.println("开始生成库外身份证号，目标数量: " + count);
        
        while (idCards.size() < count) {
            String newId = generateOutsideIdCard();
            boolean added = idCards.add(newId);
            
            if (!added) {
                consecutiveFailures++;
                if (consecutiveFailures >= maxConsecutiveFailures) {
                    System.err.println("警告：生成库外数据遇到大量重复");
                    break;
                }
            } else {
                consecutiveFailures = 0;
            }
            
            // 每1000条打印进度
            if (idCards.size() % 1000 == 0) {
                System.out.println("库外数据生成: " + idCards.size() + " / " + count);
            }
        }
        
        long totalTime = System.currentTimeMillis() - startTime;
        System.out.println("库外数据生成完成！共 " + idCards.size() + " 条，耗时: " + (totalTime / 1000.0) + " 秒");
        
        return idCards;
    }

    /**
     * 生成中文姓名
     */
    public static String generateName() {
        String[] surnames = {"张", "王", "李", "赵", "刘", "陈", "杨", "黄", "周", "吴", 
                             "徐", "孙", "马", "朱", "胡", "林", "郭", "何", "高", "罗"};
        String[] names = {"伟", "芳", "娜", "秀英", "敏", "静", "丽", "强", "磊", "军", 
                          "洋", "勇", "艳", "杰", "涛", "明", "超", "秀兰", "霞", "平"};
        
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

-- 创建数据库
CREATE DATABASE IF NOT EXISTS blacklist_test 
DEFAULT CHARACTER SET utf8mb4 
DEFAULT COLLATE utf8mb4_unicode_ci;

USE blacklist_test;

-- 黑名单主表
DROP TABLE IF EXISTS blacklist_main;
CREATE TABLE blacklist_main (
    user_id BIGINT NOT NULL AUTO_INCREMENT COMMENT '用户唯一标识（主键，自增）',
    id_card VARCHAR(20) NOT NULL COMMENT '身份证号（最后一位固定为X）',
    name VARCHAR(50) NOT NULL COMMENT '用户姓名',
    risk_level VARCHAR(20) NOT NULL COMMENT '行为评级（高风险、中风险、低风险）',
    record_count INT NOT NULL COMMENT '行为记录数（1-3）',
    PRIMARY KEY (user_id),
    UNIQUE KEY uk_id_card (id_card),
    INDEX idx_risk_level (risk_level)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='黑名单主表';

-- 行为记录表
DROP TABLE IF EXISTS behavior_record;
CREATE TABLE behavior_record (
    record_id BIGINT NOT NULL AUTO_INCREMENT COMMENT '行为记录唯一标识（主键，自增）',
    user_id BIGINT NOT NULL COMMENT '关联黑名单主表的user_id（外键）',
    behavior_type VARCHAR(30) NOT NULL COMMENT '行为类型（如"违规操作"、"异常登录"等）',
    occur_time DATETIME NOT NULL COMMENT '发生时间',
    occur_location VARCHAR(50) NOT NULL COMMENT '发生地点（如"安检通道"、"隔离区"）',
    description VARCHAR(200) NOT NULL COMMENT '行为描述',
    PRIMARY KEY (record_id),
    INDEX idx_user_id (user_id),
    INDEX idx_occur_time (occur_time),
    CONSTRAINT fk_behavior_user FOREIGN KEY (user_id) 
        REFERENCES blacklist_main(user_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='行为记录表';

-- 测试集表
DROP TABLE IF EXISTS test_set;
CREATE TABLE test_set (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT '测试集记录ID（主键，自增）',
    id_card VARCHAR(20) NOT NULL COMMENT '身份证号（明文）',
    encrypted_id_card TEXT COMMENT '加密后的身份证号',
    is_inside TINYINT NOT NULL DEFAULT 0 COMMENT '是否库内数据（1-库内，0-库外）',
    match_result TINYINT DEFAULT NULL COMMENT '查询结果（1-匹配，0-不匹配，null-未查询）',
    PRIMARY KEY (id),
    INDEX idx_is_inside (is_inside),
    INDEX idx_match_result (match_result)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='测试集表';

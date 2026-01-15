package com.blacklist;

import org.mybatis.spring.annotation.MapperScan;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

/**
 * 黑名单测试工具主启动类
 */
@SpringBootApplication
@MapperScan("com.blacklist.mapper")
public class BlacklistApplication {

    public static void main(String[] args) {
        SpringApplication.run(BlacklistApplication.class, args);
        System.out.println("========================================");
        System.out.println("黑名单查询测试工具启动成功！");
        System.out.println("访问地址: http://localhost:8080/api");
        System.out.println("========================================");
    }
}

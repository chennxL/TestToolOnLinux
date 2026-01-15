package com.blacklist.dto;

import lombok.Data;

/**
 * 测试集查询结果DTO
 */
@Data
public class QueryResultDTO {
    private String encryptedResult;  // Base64编码的加密结果（返回给Qt解密）
    private Integer matchCount;      // 匹配数量（从C++服务器返回）
    private Integer totalCount;      // 总测试数量
}

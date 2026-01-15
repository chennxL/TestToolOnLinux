package com.blacklist.dto;

import lombok.Data;

@Data
public class QueryRequestParam {
    private String payloadData;  // Base64编码的加密负载
    private String contextData;  // Base64编码的上下文
    // ========== 新增：PSI加密参数 ==========
    private Integer weight;         // PSI参数：weight（应为15）
    private Integer effectiveLambda; // PSI参数：effective_lambda（应为16）
    private Integer logPolyMod;     // PSI参数：log_poly_mod（应为14）
}

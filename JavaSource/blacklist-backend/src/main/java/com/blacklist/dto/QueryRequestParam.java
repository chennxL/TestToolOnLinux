package com.blacklist.dto;

import lombok.Data;

@Data
public class QueryRequestParam {
    private String payloadData;  // Base64编码的加密负载
    private String contextData;  // Base64编码的上下文
}

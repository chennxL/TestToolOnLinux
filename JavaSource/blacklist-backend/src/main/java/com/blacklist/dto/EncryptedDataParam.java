package com.blacklist.dto;

import lombok.Data;

@Data
public class EncryptedDataParam {
    private String payloadData;  // Base64编码的加密负载数据
    private String contextData;  // Base64编码的上下文数据
}

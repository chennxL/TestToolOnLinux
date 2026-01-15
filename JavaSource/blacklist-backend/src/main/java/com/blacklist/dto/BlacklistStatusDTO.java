package com.blacklist.dto;

import lombok.Data;

/**
 * 黑名单状态DTO
 */
@Data
public class BlacklistStatusDTO {
    
    /**
     * 状态（未创建、已创建等）
     */
    private String status;
    
    /**
     * 黑名单规模
     */
    private Long size;
}

package com.blacklist.controller;

import com.blacklist.common.Result;
import com.blacklist.dto.BlacklistCreateDTO;
import com.blacklist.dto.BlacklistStatusDTO;
import com.blacklist.service.BlacklistService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

import javax.validation.constraints.Max;
import javax.validation.constraints.Min;
import java.util.Map;

/**
 * 黑名单Controller
 */
@Slf4j
@Validated
@RestController
@RequestMapping("/blacklist")
@CrossOrigin
public class BlacklistController {

    @Autowired
    private BlacklistService blacklistService;

    /**
     * 创建黑名单
     */
    @PostMapping("/create")
    public Result<Void> createBlacklist(@RequestBody BlacklistCreateDTO param) {
        log.info("收到创建黑名单请求，规模: {}", param.getSize());
        blacklistService.createBlacklist(param.getSize());
        return Result.success();
    }

    /**
     * 获取黑名单状态
     */
    @GetMapping("/status")
    public Result<BlacklistStatusDTO> getStatus() {
        BlacklistStatusDTO status = blacklistService.getStatus();
        return Result.success(status);
    }
    
    @GetMapping("/count")
    public Result<Long> getCount(){
    try {
            Long count = blacklistService.getCount();
            log.info("查询黑名单数量: {}", count);
            return Result.success(count);
        } catch (Exception e) {
            log.error("获取黑名单数量失败", e);
            return Result.error(e.getMessage());
        }
    }
}

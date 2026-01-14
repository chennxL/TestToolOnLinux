package com.blacklist.controller;

import com.blacklist.common.Result;
import com.blacklist.dto.EncryptedDataParam;
import com.blacklist.dto.QueryRequestParam;
import com.blacklist.dto.QueryResultDTO;
import com.blacklist.dto.TestSetCreateParam;
import com.blacklist.service.TestSetService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletResponse;
import java.util.List;
import java.util.Map;

/**
 * 测试集Controller
 */
@Slf4j
@Validated
@RestController
@RequestMapping("/testset")
@CrossOrigin // 允许跨域
public class TestSetController {

    @Autowired
    private TestSetService testSetService;

    /**
     * 创建测试集
     */
    @PostMapping("/create")
    public Result<List<String>> createTestSet(@RequestBody TestSetCreateParam params) {
        log.info("收到创建测试集请求，库内: {}, 库外: {}", params.getInsideSize(), params.getOutsideSize());
        return Result.success(testSetService.createTestSet(params.getInsideSize(), params.getOutsideSize()));
    }

    /**
     * 保存加密后的测试集数据
     */
    @PostMapping("/saveEncrypted")
    public Result<Void> saveEncryptedData(@RequestBody EncryptedDataParam params) {
        log.info("收到加密数据，payload长度: {}, context长度: {}",
                params.getPayloadData().length(),
                params.getContextData().length());

        testSetService.saveEncryptedData(params.getPayloadData(), params.getContextData());
        return Result.success(null);
    }

    /**
     * 黑名单查询
     */
    @PostMapping("/query")
    public Result<QueryResultDTO> queryBlacklist(@RequestBody QueryRequestParam params) {
        log.info("收到查询请求");

        if (params.getPayloadData() == null || params.getPayloadData().isEmpty()) {
            return Result.error(400, "加密负载数据不能为空");
        }
        if (params.getContextData() == null || params.getContextData().isEmpty()) {
            return Result.error(400, "上下文数据不能为空");
        }

        log.info("Payload长度: {}, Context长度: {}",
                params.getPayloadData().length(),
                params.getContextData().length());

        try {
            QueryResultDTO result = testSetService.queryBlacklist(
                    params.getPayloadData(),
                    params.getContextData()
            );
            return Result.success(result);
        } catch (Exception e) {
            log.error("查询失败", e);
            return Result.error(500, "查询失败: " + e.getMessage());
        }
    }

}

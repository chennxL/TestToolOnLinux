package com.blacklist.service;

import com.blacklist.dto.QueryResultDTO;

import javax.servlet.http.HttpServletResponse;
import java.util.List;

/**
 * 测试集Service接口
 */
public interface TestSetService {

    /**
     * 创建测试集
     * @param insideSize 库内规模
     * @param outsideSize 库外规模
     */
    List<String> createTestSet(Integer insideSize, Integer outsideSize);


    /**
     * 保存加密后的数据
     */
    void saveEncryptedData(String payloadData, String contextData);
    /**
     * 查询黑名单
     * @return 查询结果
     */
    QueryResultDTO queryBlacklist(String payloadData, String contextData);


        /**
         * 导出查询结果
         * @param response HTTP响应
         */
//    void exportResults(HttpServletResponse response);
}

package com.blacklist.grpc;

import com.blacklist.config.GrpcConfig;
import com.blacklist.dto.BlacklistFullInfo;
import com.blacklist.util.BlacklistBitEncoder;
import com.blacklist.util.IdCardHashUtil;
import com.blacklist.util.IdCompress;  // ⭐ 修改：替换导入
import com.google.protobuf.ByteString;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import lombok.extern.slf4j.Slf4j;
import lombok.var;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import psi.PSIServiceGrpc;
import psi.Psi;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.util.Base64;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;


@Slf4j
@Component
public class PSIGrpcClient {

    @Autowired
    private GrpcConfig grpcConfig;

    private ManagedChannel channel;
    private PSIServiceGrpc.PSIServiceBlockingStub blockingStub;

    @PostConstruct
    public void init() {
        log.info("初始化gRPC客户端，连接到 {}:{}", grpcConfig.getAddress(), grpcConfig.getPort());

        channel = ManagedChannelBuilder
                .forAddress(grpcConfig.getAddress(), grpcConfig.getPort())
                .usePlaintext()
                .maxInboundMessageSize(100 * 1024 * 1024)  // 100MB
                .build();

        blockingStub = PSIServiceGrpc.newBlockingStub(channel);

        log.info("gRPC客户端初始化完成");
    }

    @PreDestroy
    public void shutdown() {
        try {
            if (channel != null && !channel.isShutdown()) {
                channel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
                log.info("gRPC客户端已关闭");
            }
        } catch (InterruptedException e) {
            log.error("关闭gRPC客户端失败", e);
            Thread.currentThread().interrupt();
        }
    }

    /**
     * 执行PSI匹配（使用位编码方案）
     */
    public String doMatch(String contextData, String payloadData, List<BlacklistFullInfo> blacklistData,
                      int weight, int effectiveLambda, int logPolyMod) {
        try {
            log.info("========================================");
            log.info("开始gRPC调用");
            log.info("黑名单数据量: {}", blacklistData.size());

            // 1. 解码Base64数据
            byte[] contextBytes = Base64.getDecoder().decode(contextData);
            byte[] payloadBytes = Base64.getDecoder().decode(payloadData);

            log.info("Context字节数: {}", contextBytes.length);
            log.info("Payload字节数: {}", payloadBytes.length);

            // 2. 转换黑名单数据为srv_data格式
            log.info("开始转换srv_data...");
            Map<Long, Psi.LabelsType> srvData = convertBlacklistToSrvData(blacklistData);

            log.info("srv_data大小: {}", srvData.size());
            // ✅ 立即清理blacklistData（已经转换完了）
            blacklistData.clear();
            // 4. 构建请求
            log.info("构建gRPC请求...");
            log.info("PSI参数 - weight: {}, effectiveLambda: {}, logPolyMod: {}",
                    weight, effectiveLambda, logPolyMod);


            Psi.MatchRequest request = Psi.MatchRequest.newBuilder()
                    .setContextData(ByteString.copyFrom(contextBytes))
                    .setPayloadData(ByteString.copyFrom(payloadBytes))
                    .putAllSrvData(srvData)
                    .setWeight(weight)                      // 新增
                    .setEffectiveLambda(effectiveLambda)    // 新增
                    .setLogPolyMod(logPolyMod)              // 新增
                    .build();

            log.info("请求大小: {} 字节", request.getSerializedSize());

            // ✅ 立即清理srvData（已经复制到request中了）
            srvData.clear();
            srvData = null;

            // 6. 调用gRPC（设置15分钟超时）
            Psi.EncryptResponse response = blockingStub
                    .withDeadlineAfter(15, TimeUnit.MINUTES)
                    .doMatch(request);

            // 7. 返回Base64编码的结果
            byte[] resultBytes = response.getPayloadData().toByteArray();
            String resultBase64 = Base64.getEncoder().encodeToString(resultBytes);

            log.info("gRPC调用成功！");
            log.info("返回结果字节数: {}", resultBytes.length);
            log.info("========================================");

            return resultBase64;

        } catch (Exception e) {
            log.error("========================================");
            log.error("gRPC调用失败: {}", e.getMessage());
            log.error("异常类型: {}", e.getClass().getName());
            if (e.getCause() != null) {
                log.error("原因: {}", e.getCause().getMessage());
            }
            log.error("========================================");
            log.error("详细堆栈:", e);
            throw new RuntimeException("PSI匹配失败: " + e.getMessage(), e);
        }
    }

    /**
     * 将黑名单完整信息转换为srv_data格式（使用位编码）
     *
     * Map结构：
     * key = 身份证号哈希值
     * value = LabelsType { labels: [编码后的完整信息] }
     */
    /**
     * 将黑名单完整信息转换为srv_data格式（使用压缩编码）
     *
     * Map结构：
     * key = 身份证号压缩后的整数（可逆编码，零碰撞）
     * value = LabelsType { labels: [编码后的完整信息] }
     */
    private Map<Long, Psi.LabelsType> convertBlacklistToSrvData(List<BlacklistFullInfo> blacklistData) {
        Map<Long, Psi.LabelsType> srvData = new HashMap<>();

        log.info("---------- srv_data 转换详情（使用压缩编码）----------");

        for (int i = 0; i < blacklistData.size(); i++) {
            BlacklistFullInfo info = blacklistData.get(i);

            try {
                // ⭐ 关键修改：使用压缩编码替代哈希
                long compressedId = IdCompress.compress(info.getMain().getIdCard());

                // 编码为labels数组
                long[] labels = BlacklistBitEncoder.encodeBlacklistInfoToLabels(info);

                // 构建LabelsType
                Psi.LabelsType.Builder labelsBuilder = Psi.LabelsType.newBuilder();
                for (long label : labels) {
                    labelsBuilder.addLabels(label);
                }
                Psi.LabelsType labelsType = labelsBuilder.build();

                srvData.put(compressedId, labelsType);

                // 打印前3条的详细信息
                if (i < 3) {
                    log.info("  [{}] 身份证={}, compressed={}",
                            i, info.getMain().getIdCard(), compressedId);
                    log.info("       可逆验证: {}", 
                            IdCompress.decompress(compressedId).equals(info.getMain().getIdCard()) ? "✓" : "✗");
                    log.info("       labels数量: {}", labels.length);
                    log.info("       labels[0]: {} (评级={}, 记录数={})",
                            labels[0],
                            info.getMain().getRiskLevel().getDescription(),
                            info.getMain().getRecordCount());

                    for (int j = 0; j < info.getRecords().size(); j++) {
                        var rec = info.getRecords().get(j);
                        log.info("       labels[{}]: {} ({}(code={}) + {}(code={}))",
                                j + 1,
                                labels[j + 1],
                                rec.getBehaviorType().getDescription(),
                                rec.getBehaviorType().getCode(),
                                rec.getTool().getDescription(),
                                rec.getTool().getCode());
                    }
                }
            } catch (Exception e) {
                log.error("处理身份证号失败: {}", info.getMain().getIdCard(), e);
                // 跳过这条数据，继续处理其他
            }
        }

        log.info("--------------------------------------");
        log.info("srv_data转换完成，共 {} 条", srvData.size());

        return srvData;
    }
}

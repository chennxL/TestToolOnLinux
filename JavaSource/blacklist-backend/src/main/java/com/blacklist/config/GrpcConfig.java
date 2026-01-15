package com.blacklist.config;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

@Data
@Configuration
@ConfigurationProperties(prefix = "grpc.server")
public class GrpcConfig {
    private String address;
    private Integer port;
}

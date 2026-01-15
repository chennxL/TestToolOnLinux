#ifndef PSI_SERVER_H
#define PSI_SERVER_H
#pragma once

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "psicommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Server_Context_t
 * @brief 服务端上下文句柄
 * * 存储服务端运行所需的参数、加密上下文或预计算的查找表。
 */
typedef struct Server_Context_t Server_Context_t;

/**
 * @brief 创建服务端上下文
 * * 根据传入的初始化数据（通常是客户端发送的上下文流或公钥信息）来初始化服务端环境。
 * * @param data 指向初始化数据缓冲区的指针
 * @param count 数据缓冲区的字节长度
 * @return Server_Context_t* 返回创建的服务端上下文指针，若创建失败则返回 NULL
 */
Server_Context_t *PSI_Server_Context_Create(const char *data, size_t count);

/**
 * @brief 执行匹配计算
 * * 在服务端执行核心业务逻辑（如隐私求交、模式匹配等）。它接收加密的负载数据，
 * 并返回一个包含计算结果的字节流。
 * * @param context 服务端上下文指针
 * @param data 接收到的客户端负载（Payload）数据
 * @param count 负载数据的长度
 * @return C_Stream* 返回包含计算结果的流指针，调用者需负责后续的销毁工作
 * @see Stream_Destroy
 */
C_Stream *PSI_Server_Do_Matching(Server_Context_t *context, const char *data,
                             size_t count);

/**
 * @brief 销毁服务端上下文并释放资源
 * * 释放与该上下文关联的所有内存、密钥及内部缓存。
 * * @param context 待销毁的服务端上下文指针
 */
void PSI_Server_Context_Destory(Server_Context_t *context);

#ifdef __cplusplus
}
#endif

#endif // PSI_SERVER_H
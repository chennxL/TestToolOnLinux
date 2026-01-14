#ifndef PSI_CLIENT_H
#define PSI_CLIENT_H
#pragma once

#ifdef __cplusplus
#include <cstddef>
#else
#include "stddef.h"
#endif

#include "psicommon.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @struct Client_Context_t
 * @brief 客户端上下文句柄，用于存储加密参数和状态
 */
typedef struct Client_Context_t Client_Context_t;
/**
 * @struct Reveal_Table
 * @brief 揭示表（Reveal Table）结构，用于在解密阶段映射结果
 */
typedef struct Reveal_Table Reveal_Table;

/**
 * @struct Reveal_Result
 * @brief 解密后的结果结构体
 */
typedef struct Reveal_Result {
  size_t key;    /**< 结果对应的键 */
  size_t *value; /**< 指向结果数值数组的指针 */
  size_t count;  /**< 数值数组中的元素个数 */
#ifdef __cplusplus
  /** @brief C++ 析构函数，用于自动释放内存 */
  ~Reveal_Result() { delete[] value; }
#endif
} Reveal_Result;

/**
 * @brief 创建客户端上下文
 * * @param weight 权重参数
 * @param effective_lambda 有效安全性参数 (lambda)
 * @param log_poly_mod 多项式模数的对数值
 * @return Client_Context_t* 返回创建的上下文指针，失败返回 NULL
 */
Client_Context_t *PSI_Client_Context_Create(size_t weight, size_t effective_lambda,
                                        size_t log_poly_mod);

/**
 * @brief 将客户端上下文序列化为流
 * * @param context 客户端上下文指针
 * @return C_Stream* 返回序列化后的流指针
 */
C_Stream *PSI_Client_Context_To_Stream(Client_Context_t *context);

/**
 * @brief 打包原始数据为加密负载，并生成对应的揭示表
 * * @param context 客户端上下文指针
 * @param data 待打包的原始数据数组
 * @param count 数据的元素个数
 * @param reveal_table [out] 用于接收生成的揭示表指针的指针
 * @return C_Stream* 返回加密后的负载流指针
 */
C_Stream *PSI_Client_Pack_Payload(Client_Context_t *context, const size_t *data,
                              size_t count, Reveal_Table **reveal_table);

/**
 * @brief 揭示（解密）运算结果
 * * @param context 客户端上下文指针
 * @param reveal_table 打包阶段生成的揭示表指针
 * @param data 接收到的加密结果数据缓冲区
 * @param count 数据缓冲区的长度
 * @param result_count [out] 用于接收解密后结果条数的指针
 * @return Reveal_Result* 返回解密后的结果结构体数组指针
 */
Reveal_Result *PSI_Client_Reveal_Result(Client_Context_t *context,
                                    const Reveal_Table *reveal_table,
                                    const char *data, size_t count,
                                    size_t *result_count);

/**
 * @brief 销毁揭示表并释放内存
 * @param reveal_table 待销毁的揭示表指针
 */
void PSI_Reveal_Table_Destory(Reveal_Table *reveal_table);

/**
 * @brief 销毁解密结果数组并释放内存
 * @param reveal_result 待销毁的结果数组指针
 */
void PSI_Reveal_Result_Destory(Reveal_Result *reveal_result);

/**
 * @brief 销毁客户端上下文并释放资源
 * @param context 待销毁的上下文指针
 */
void PSI_Client_Context_Destory(Client_Context_t *context);

#ifdef __cplusplus
}
#endif

#endif // PSI_CLIENT_H

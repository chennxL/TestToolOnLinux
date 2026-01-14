#ifndef PSI_COMMON_H
#define PSI_COMMON_H
#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <sstream>
#include <string>
#else
#include "stddef.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct C_Stream
 * @brief 字节流封装结构体
 * * 该结构体用于处理数据的序列化与反序列化。
 * - 在 C++ 环境下：利用 std::stringstream 实现，支持内存自动管理。
 * - 在 C 环境下：作为不透明句柄（Opaque Handle）使用。
 */
typedef struct C_Stream
#ifdef __cplusplus
{
  std::stringstream ss;    /**< 内部使用的字符串流，用于存储和操作数据 */
  std::string last_output; /**< 缓存最后一次读取输出的字符串 */
} C_Stream;
#else
    C_Stream;
#endif

/**
 * @brief 从流中读取数据
 * * 从给定的 C_Stream 中提取字节数据。通常返回指向内部缓冲区的指针。
 * * @param s 指向 C_Stream 实例的指针
 * @param out_length [out] 用于接收读取到的数据长度（字节数）
 * @return const char*
 * 返回指向数据缓冲区的指针。注意：该指针的生命周期通常受流的控制。
 */
const char *PSI_Stream_Read(C_Stream *s, size_t *out_length);

/**
 * @brief 销毁流对象并释放资源
 * * 释放 C_Stream
 * 占用的所有内存。调用此函数后，指向该流及其内部缓冲区的指针将失效。
 * * @param s 指向待销毁 C_Stream 实例的指针
 */
void PSI_Stream_Destroy(C_Stream *s);

const char *PSI_Version();

#ifdef __cplusplus
}
#endif
#endif // PSI_COMMON_H
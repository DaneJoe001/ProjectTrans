/**
 * @file buffer.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 网络缓冲区类型别名
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供网络模块通用的字节缓冲区类型别名 Buffer。
 *          该类型通常用于承载网络收发的原始字节序列。
 */
#pragma once

#include <vector>
#include <cstdint>

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 字节缓冲区
     * @details 用于承载原始字节数据的连续存储容器。
     */
    using Buffer = std::vector<uint8_t>;
}
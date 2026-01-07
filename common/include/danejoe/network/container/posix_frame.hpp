/**
 * @file posix_frame.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief POSIX 传输帧
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义在 POSIX 网络收发/传输流程中使用的帧结构 PosixFrame。
 *          PosixFrame 用于将连接标识与其对应的字节数据载荷打包传递。
 */
#pragma once

#include "danejoe/network/container/buffer.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    /**
     * @struct PosixFrame
     * @brief POSIX 传输帧
     * @details 由连接标识与数据载荷组成的轻量结构体，通常用于线程/队列间传递。
     */
    struct PosixFrame
    {
        /// @brief 连接标识（由上层连接管理模块分配）
        uint64_t connect_id;
        /// @brief 帧数据载荷（原始字节序列）
        Buffer data;
    };
#endif
};
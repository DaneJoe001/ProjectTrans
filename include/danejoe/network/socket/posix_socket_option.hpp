/**
 * @file posix_socket_option.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Posix选项
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#if defined (__linux__)

#include <cstdint>

#include <sys/socket.h>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @struct PosixSocketOption
     * @brief Posix选项
     */
    struct PosixSocketOption
    {
        /// @brief 选项级别
        int32_t level;
        /// @brief 选项名称
        int32_t opt_name;
        /// @brief 选项值
        void* opt_val;
        /// @brief 选项长度
        socklen_t opt_len;
    };
}

#endif

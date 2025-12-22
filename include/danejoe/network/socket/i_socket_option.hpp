/**
 * @file i_socket_option.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 套接字选项接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @struct ISocketOption
     * @brief 套接字选项接口
     */
    struct ISocketOption
    {
        /// @brief 选项级别
        int32_t level;
        /// @brief 选项名称
        int32_t opt_name;
        /// @brief 选项值
        void* opt_value = nullptr;
        /// @brief 选项长度
        int32_t opt_length;
    };
}

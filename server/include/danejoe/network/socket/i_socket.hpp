/**
 * @file i_socket.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 套接字接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>

#include "danejoe/network/socket/i_socket_option.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISocket
     * @brief 套接字接口
     */
    class ISocket
    {
    public:
        virtual ~ISocket() = default;
        /**
         * @brief 获取原始套接字
         * @return 原始套接字句柄
         */
        virtual const void* get_raw_socket()const = 0;
        /**
         * @brief 判断套接字是否有效
         * @return true 套接字有效
         * @return false 套接字无效
         */
        virtual bool is_valid()const = 0;
        /**
         * @brief 获取逻辑标识符
         * @return 逻辑标识符
         */
        virtual int32_t get_id()const = 0;
        /**
         * @brief 设置选项
         * @param option 选项
         * @return true 设置成功
         * @return false 设置失败
         */
        virtual bool set_opt(const ISocketOption& option) = 0;
        /**
         * @brief 设置非阻塞模式
         * @param status 是否为非阻塞模式
         * @return true 设置成功
         * @return false 设置失败
         */
        virtual bool set_non_blocking(bool status) = 0;
        /**
         * @brief 关闭套接字
         */
        virtual void close() = 0;
        /**
         * @brief 检查套接字是否可读
         * @return true 套接字可读
         * @return false 套接字不可读
         */
        virtual bool is_readable()const = 0;
        /**
         * @brief 检查套接字是否可写
         * @return true 套接字可写
         * @return false 套接字不可写
         */
        virtual bool is_writable()const = 0;
    protected:
        /// @brief 是否为非阻塞模式
        bool m_is_non_blocking = false;
    };

}

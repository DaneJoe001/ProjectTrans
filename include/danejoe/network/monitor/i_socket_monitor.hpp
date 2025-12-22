/**
 * @file i_socket_monitor.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 套接字监控器接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include "danejoe/network/socket/i_socket.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISocketMonitor
     * @brief 套接字监控器接口
     */
    class ISocketMonitor
    {
    public:
        /**
         * @brief 检查套接字是否可读
         * @param socket 套接字
         * @return true 套接字可读
         * @return false 套接字不可读
         */
        virtual bool is_readable(const ISocket* socket) = 0;
        /**
         * @brief 检查套接字是否可写
         * @param socket 套接字
         * @return true 套接字可写
         * @return false 套接字不可写
         */
        virtual bool is_writable(const ISocket* socket) = 0;
    };
}



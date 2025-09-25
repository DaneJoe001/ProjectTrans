#pragma once

/**
 * @file i_socket_monitor.hpp
 * @brief 套接字监控器接口
 * @author DaneJoe001
 */

#include "common/network/i_socket.hpp"

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
     * @return 是否可读
     */
    virtual bool is_readable(const ISocket* socket) = 0;
    /**
     * @brief 检查套接字是否可写
     * @param socket 套接字
     * @return 是否可写
     */
    virtual bool is_writable(const ISocket* socket) = 0;
};
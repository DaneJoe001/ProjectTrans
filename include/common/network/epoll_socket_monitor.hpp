#pragma once

/**
 * @file epoll_socket_monitor.hpp
 * @brief Epoll套接字监控器
 * @author DaneJoe001
 */

#include <mutex>
#include <cstdint>

#include  "common/network/i_socket_monitor.hpp"

/**
 * @class EpollSocketMonitor
 * @brief Epoll套接字监控器
 */
class EpollSocketMonitor : public ISocketMonitor
{
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
    static ISocketMonitor* get_instance();
    /**
     * @brief 检查套接字是否可读
     * @param socket 套接字
     * @return 是否可读
     */
    bool is_readable(const ISocket* socket) override;
    /**
     * @brief 检查套接字是否可写
     * @param socket 套接字
     * @return 是否可写
     */
    bool is_writeable(const ISocket* socket) override;
private:
    /**
     * @brief 检查套接字
     * @param socket 套接字
     * @param event_type 事件类型
     * @return 是否可读或可写
     */
    bool check_socket(const ISocket* socket, uint32_t event_type);
private:
    /**
     * @brief 构造函数
     */
    EpollSocketMonitor();
    /**
     * @brief 析构函数
     */
    ~EpollSocketMonitor();
private:
    /// @brief 互斥锁
    std::mutex m_mutex;
    /// @brief 文件描述符
    int32_t m_epoll_fd = -1;

};
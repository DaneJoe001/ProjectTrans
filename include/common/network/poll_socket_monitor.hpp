#pragma once

/**
 * @file poll_socket_monitor.hpp
 * @brief Poll套接字监控器
 * @author DaneJoe001
 * @todo 修改事件检查参数为抽线事件类型
 */

#include <cstdint>

#include "common/network/i_socket_monitor.hpp"

 /**
  * @class PollSocketMonitor
  * @brief Poll套接字监控器
  */
class PollSocketMonitor : public ISocketMonitor {
public:
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
    /**
     * @brief 获取实例
     * @return 实例
     */
    static ISocketMonitor* get_instance();
private:
    /**
     * @brief 检查套接字
     * @param socket 套接字
     * @param event 事件
     * @return 是否可读或可写
     */
    bool check_socket(const ISocket* socket, int16_t  event);
    /**
     * @brief 构造函数
     */
    PollSocketMonitor();
    /**
     * @brief 析构函数
     */
    ~PollSocketMonitor();
private:

};
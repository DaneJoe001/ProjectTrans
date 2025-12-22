/**
 * @file poll_socket_monitor.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Poll套接字监控器
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once
#if defined(__linux__)

#include <cstdint>  

#include "danejoe/network/monitor/i_socket_monitor.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class PollSocketMonitor
     * @brief Poll套接字监控器
     */
    class PollSocketMonitor : public ISocketMonitor
    {
    public:
        /**
         * @brief 检查套接字是否可读
         * @param socket 套接字
         * @return true 套接字可读
         * @return false 套接字不可读
         */
        bool is_readable(const ISocket* socket) override;
        /**
         * @brief 检查套接字是否可写
         * @param socket 套接字
         * @return true 套接字可写
         * @return false 套接字不可写
         */
        bool is_writable(const ISocket* socket) override;
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
         * @return true 套接字可读或可写
         * @return false 套接字不可读且不可写
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
    };
}

#endif

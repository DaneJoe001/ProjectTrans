/**
 * @file network_runtime.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 网络运行时
 * @date 2026-01-06
 */

#pragma once

#include <memory>

#include "danejoe/network/event_loop/posix_epoll_event_loop.hpp"
#include "danejoe/network/runtime/reactor_mail_box.hpp"

/**
 * @class NetworkRuntime
 * @brief 网络运行时
 */
class NetworkRuntime
{
public:
    /**
     * @brief 构造函数
     * @param reactor_mail_box 反应器邮箱
     */
    NetworkRuntime(
        std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 是否初始化
     * @return 是否初始化
     */
    bool is_init() const;
    /**
     * @brief 运行
     */
    void run();
    /**
     * @brief 停止
     */
    void stop();
private:
    /// @brief epoll 事件循环
    DaneJoe::PosixEpollEventLoop m_event_loop;
    /// @brief 反应器邮箱
    std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;
    /// @brief 是否已初始化
    bool m_is_init = false;
};


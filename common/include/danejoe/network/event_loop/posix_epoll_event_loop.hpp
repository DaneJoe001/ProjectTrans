/**
 * @file posix_epoll_event_loop.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief epoll 事件循环
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义基于 epoll 的事件循环 PosixEpollEventLoop，用于驱动服务器监听 socket 与各连接的可读/可写事件。
 *          事件循环通常与 ReactorMailBox 协作：
 *          - 从 epoll 获取就绪事件并分发处理
 *          - 通过 eventfd/pipe 等通知机制唤醒阻塞的 epoll_wait
 */
#pragma once

#include <memory>
#include <atomic>
#include <unordered_map>

#include "danejoe/common/type_traits/platform_traits.hpp"

#include "danejoe/network/handle/posix_epoll_handle.hpp"
#include "danejoe/network/handle/posix_event_handle.hpp"
#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/context/connect_context.hpp"
#include "danejoe/network/runtime/reactor_mail_box.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    /**
     * @class PosixEpollEventLoop
     * @brief epoll 事件循环
     * @details 封装 epoll 驱动的事件分发逻辑，维护连接上下文表，并负责：
     *          - acceptable_event()：处理监听 socket 的 accept
     *          - readable_event()/writable_event()：处理连接 fd 的读写
     *          - notify_event()：处理通知事件（通常用于唤醒/处理 mailbox）
     * @note 线程模型：一般在单独线程中调用 run()，其余线程通过 notify() 请求唤醒。
     */
    class PosixEpollEventLoop
    {
    public:
        /**
         * @brief 默认构造
         */
        PosixEpollEventLoop();
        /**
         * @brief 构造并初始化事件循环
         * @param reactor_mail_box Reactor 邮箱
         * @param event_handle 通知事件句柄（用于唤醒 epoll_wait）
         * @param server_handle 服务器监听 socket 句柄（移动接管其所有权）
         * @param epoll_handle epoll 句柄（移动接管其所有权）
         */
        PosixEpollEventLoop(
            std::shared_ptr<ReactorMailBox> reactor_mail_box,
            std::shared_ptr<PosixEventHandle> event_handle,
            PosixSocketHandle&& server_handle,
            PosixEpollHandle&& epoll_handle);
        /**
         * @brief 析构
         */
        ~PosixEpollEventLoop();
        /**
         * @brief 初始化事件循环
         * @param reactor_mail_box Reactor 邮箱
         * @param event_handle 通知事件句柄（用于唤醒 epoll_wait）
         * @param server_handle 服务器监听 socket 句柄（移动接管其所有权）
         * @param epoll_handle epoll 句柄（移动接管其所有权）
         */
        void init(
            std::shared_ptr<ReactorMailBox> reactor_mail_box,
            std::shared_ptr<PosixEventHandle> event_handle,
            PosixSocketHandle&& server_handle,
            PosixEpollHandle&& epoll_handle);
        /**
         * @brief 运行事件循环
         * @details 通常为阻塞循环；直到 stop() 触发退出。
         */
        void run();
        /**
         * @brief 请求停止事件循环
         * @details 通常会配合 notify() 唤醒 run() 内部等待，使其尽快退出。
         */
        void stop();
        /**
         * @brief 唤醒事件循环
         * @details 向 event_handle 写入通知，使阻塞的 epoll_wait 立刻返回。
         */
        void notify();
        /**
         * @brief 移除连接
         * @param fd 连接对应的文件描述符
         */
        void remove_connect(int fd);
        /**
         * @brief 处理连接可读事件
         * @param fd 连接对应的文件描述符
         */
        void readable_event(int fd);
        /**
         * @brief 处理连接可写事件
         * @param fd 连接对应的文件描述符
         */
        void writable_event(int fd);
        /**
         * @brief 处理监听 socket 的可接受连接事件
         */
        void acceptable_event();
        /**
         * @brief 处理通知事件
         */
        void notify_event();
    private:
        /// @brief epoll_wait 一次拉取的最大事件数量
        int m_max_event_counts = 1024;
        /// @brief 事件循环运行标志
        std::atomic<bool> m_is_running = false;
        /// @brief 连接计数器（用于分配 connect_id）
        std::atomic<uint64_t> m_connect_counter = 0;
        /// @brief Reactor 邮箱
        std::shared_ptr<ReactorMailBox> m_reactor_mail_box = nullptr;
        /// @brief 连接上下文表（key: fd）
        std::unordered_map<int, ConnectContext> m_connect_contexts;
        /// @brief epoll 句柄
        PosixEpollHandle m_epoll_handle;
        /// @brief 通知事件句柄
        std::shared_ptr<PosixEventHandle> m_event_handle;
        /// @brief 服务器监听 socket 句柄
        PosixSocketHandle m_server_handle;
    };
#endif
}
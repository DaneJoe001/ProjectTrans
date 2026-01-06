/**
 * @file epoll_event_loop.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Epoll事件循环
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once
#if defined(__linux__)

#include <atomic>
#include <unordered_map>

#include "danejoe/network/event_loop/i_event_loop.hpp"
#include "danejoe/network/socket/posix_server_socket.hpp"
#include "danejoe/network/context/i_socket_context.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class EpollEventLoop
     * @brief Epoll事件循环
     * @note 该类当前并非线程安全。
     */
    class EpollEventLoop : public IEventLoop
    {
    public:
        /**
         * @brief 构造函数
         * @param server_socket 服务器套接字
         * @param context_creator 业务上下文创建器
         */
        EpollEventLoop();
        /**
         * @brief 析构函数
         */
        ~EpollEventLoop();
        void notify();
        void init(std::unique_ptr<PosixServerSocket> server_socket, std::unique_ptr<ISocketContextCreator> context_creator);
        /**
         * @brief 添加套接字
         * @param socket 套接字
         * @param type 事件类型
         * @return true 添加成功
         * @return false 添加失败
         */
        bool add_socket(std::unique_ptr<IClientSocket> socket, EventLoopEventType type) override;
        /**
         * @brief 移除套接字
         * @param socket_id 套接字标识符
         */
        void remove_socket(int32_t socket_id) override;
        /**
         * @brief 运行事件循环
         */
        void run() override;
        /**
         * @brief 判断事件循环是否有效
         * @return true 事件循环有效
         * @return false 事件循环无效
         */
        bool is_valid() override;
        /**
         * @brief 转换事件类型至epoll事件类型
         * @param type 事件类型
         * @return epoll事件类型
         */
        uint32_t to_epoll_events(EventLoopEventType type);
        /**
         * @brief 接受事件
         */
        void acceptable_event();
        /**
         * @brief 可读事件
         * @param fd 文件描述符
         * @note 只处理数据的接收，缓存区会被共享到业务上下文
         */
        void readable_event(int32_t socket_id);
        /**
         * @brief 可写事件
         * @param fd 文件描述符
         * @note 只处理数据的发送，缓存区会被共享到业务上下文
         */
        void writable_event(int32_t socket_id);
        /**
         * @brief 停止事件循环
         */
        void stop()override;
    private:
        void reset_sign();
    private:
        /// @brief 是否运行
        std::atomic<bool> m_is_running = false;
        bool m_is_init = false;
        /// @brief 最大事件数
        int32_t m_max_event_count = 1024;
        /// @brief epoll文件描述符
        int32_t m_epoll_fd = -1;
        int32_t m_event_fd = -1;
        /// @brief 服务器套接字
        std::unique_ptr<PosixServerSocket> m_server_socket;
        /// @brief 接收缓存
        /// @tparam int32_t 逻辑socket标识符
        /// @tparam std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> 接收缓存
        /// @tparam int32_t 逻辑socket标识符
        std::unordered_map<int32_t, std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>>> m_recv_buffers;
        /// @brief 发送缓存
        /// @tparam int32_t 逻辑socket标识符
        /// @tparam std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> 发送缓存
        std::unordered_map<int32_t, std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>>> m_send_buffers;
        /// @brief 套接字
        /// @tparam int32_t 逻辑socket标识符
        /// @tparam std::unique_ptr<IClientSocket> 套接字
        std::unordered_map<int32_t, std::unique_ptr<IClientSocket>> m_sockets;
        /// @brief 业务上下文
        /// @tparam int32_t 逻辑socket标识符
        /// @tparam std::shared_ptr<ISocketContext> 业务上下文
        std::unordered_map<int32_t, std::shared_ptr<ISocketContext>> m_contexts;
        /// @brief 套接字事件
        /// @tparam int32_t 逻辑socket标识符
        /// @tparam EventLoopEventType 事件类型
        std::unordered_map<int32_t, EventLoopEventType> m_socket_events;
        /// @brief 业务上下文创建器
        /// @tparam std::unique_ptr<ISocketContextCreator> 业务上下文创建器
        std::unique_ptr<ISocketContextCreator> m_context_creator;
    };
}

#endif


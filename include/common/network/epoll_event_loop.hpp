#pragma once

/**
 * @file epoll_event_loop.hpp
 * @brief Epoll事件循环
 * @author DaneJoe001
 */

#include <functional>

#include "common/network/i_event_loop.hpp"
#include "common/network/posix_server_socket.hpp"
#include "common/network/i_socket_context.hpp"

/**
 * @class EpollEventLoop
 * @brief EpollEventLoop类实现了IEventLoop接口，使用epoll作为事件循环机制。
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
    EpollEventLoop(std::unique_ptr<PosixServerSocket> server_socket, std::unique_ptr<ISocketContextCreator> context_creator);
    /**
     * @brief 析构函数
     */
    ~EpollEventLoop();
    /**
     * @brief 添加套接字
     * @param socket 套接字
     * @param type 事件类型
     * @return 是否成功
     */
    bool add_socket(std::unique_ptr<IClientSocket> socket, EventType type) override;
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
     * @return 是否有效
     */
    bool is_valid() override;
    /**
     * @brief 转换事件类型至epoll事件类型
     * @param type 事件类型
     * @return epoll事件类型
     */
    uint32_t to_epoll_events(EventType type);
    /**
     * @brief 接受事件
     */
    void acceptable_event();
    /**
     * @brief 可读事件
     * @param fd 文件描述符
     */
    void readable_event(int32_t fd);
    /**
     * @brief 可写事件
     * @param fd 文件描述符
     */
    void writable_event(int32_t fd);
    /**
     * @brief 停止事件循环
     */
    void stop()override;
private:
    /// @brief 最大事件数
    int32_t m_max_event_account = 1024;
    /// @brief epoll文件描述符
    int32_t m_epoll_fd = -1;
    /// @brief 服务器套接字
    std::unique_ptr<PosixServerSocket> m_server_socket;
    /// @brief 接收缓存
    /// @tparam int32_t 逻辑socket标识符
    /// @tparam std::shared_ptr<DaneJoe::MTQueue<uint8_t>> 接收缓存
    /// @tparam int32_t 逻辑socket标识符
    std::unordered_map<int32_t, std::shared_ptr<DaneJoe::MTQueue<uint8_t>>> m_recv_buffers;
    /// @brief 发送缓存
    /// @tparam int32_t 逻辑socket标识符
    /// @tparam std::shared_ptr<DaneJoe::MTQueue<uint8_t>> 发送缓存
    std::unordered_map<int32_t, std::shared_ptr<DaneJoe::MTQueue<uint8_t>>> m_send_buffers;
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
    /// @tparam EventType 事件类型
    std::unordered_map<int32_t, EventType> m_socket_events;
    /// @brief 业务上下文创建器
    /// @tparam std::unique_ptr<ISocketContextCreator> 业务上下文创建器
    std::unique_ptr<ISocketContextCreator> m_context_creator;
};
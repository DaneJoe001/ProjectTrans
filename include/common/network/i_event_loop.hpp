#pragma once

#include <memory>
#include <list>
#include <unordered_map>

#include "common/network/i_client_socket.hpp"
#include "mt_queue/mt_queue.hpp"
#include "common/network/i_server_socket.hpp"
#include "common/network/i_socket_context.hpp"

/**
 * @class IEventLoop
 * @brief IEventLoop类实现了IEventLoop接口，用于表示事件循环。
 */
class IEventLoop
{
public:
    /**
     * @enum EventType
     * @brief 事件类型
     * @note 当前事件类型设置源于epoll事件类型定义
     */
    enum class EventType :uint32_t
    {
        /// @brief 无事件
        None = 0,
        /// @brief 可读事件
        Readable = 1 << 0,
        /// @brief 可写事件
        Writable = 1 << 1,
        /// @brief 优先级事件
        Priority = 1 << 2,
        /// @brief 断开连接事件
        Disconnected = 1 << 3,
        /// @brief 边缘触发事件
        EdgeTriggered = 1 << 4,
        /// @brief 一次事件
        OneShot = 1 << 5,
        /// @brief 错误事件
        Error = 1 << 6,
        /// @brief 对端关闭事件
        PeerClosed = 1 << 7,
    };
public:
    /**
     * @brief 构造函数
     */
    IEventLoop() = default;
    /**
     * @brief 判断事件是否有效
     * @param type 事件类型
     * @return 是否有效
     */
    static bool is_event_valid(EventType type);
    /**
     * @brief 运行事件循环
     */
    virtual void run() = 0;
    /**
     * @brief 添加套接字
     * @param socket 套接字
     * @param type 事件类型
     * @return 是否成功
     */
    virtual bool add_socket(std::unique_ptr<IClientSocket> socket, EventType type) = 0;
    /**
     * @brief 移除套接字
     * @param socket_id socket逻辑标识符
     */
    virtual void remove_socket(int socket_id) = 0;
    /**
     * @brief 停止事件循环
     */
    virtual void stop() = 0;
    /**
     * @brief 判断事件循环是否有效
     * @return 是否有效
     */
    virtual bool is_valid() = 0;
protected:
};

/**
 * @brief 事件类型位运算
 * @param lhs 左操作数
 * @param rhs 右操作数
 */
IEventLoop::EventType operator|(IEventLoop::EventType lhs, IEventLoop::EventType rhs);

/**
 * @brief 事件类型位运算
 * @param lhs 左操作数
 * @param rhs 右操作数
 */
IEventLoop::EventType operator&(IEventLoop::EventType lhs, IEventLoop::EventType rhs);
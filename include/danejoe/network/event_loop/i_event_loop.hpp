/**
 * @file i_event_loop.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 事件循环接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <atomic>
#include <memory>
#include <cstdint>

#include "danejoe/network/socket/i_client_socket.hpp"
#include "danejoe/network/socket/i_server_socket.hpp"
#include "danejoe/common/type_traits/enum_type_traits.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @enum EventLoopEventType
     * @brief 事件类型
     * @note 当前事件类型设置源于epoll事件类型定义
     */
    enum class EventLoopEventType :uint32_t
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
    /**
     * @brief 启用位掩码操作符
     */
    template<>
    struct enable_bitmask_operator<EventLoopEventType> :std::true_type {};
    /**
     * @brief 获取枚举字符串
     * @param type 事件类型
     * @note 用于日志输出
     */
    std::string to_string(EventLoopEventType type);
    /**
     * @brief 从字符串转换为枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 EventLoopEventType::None
     */
    template<>
    EventLoopEventType enum_cast<EventLoopEventType>(const std::string& enum_string);
    /**
     * @class IEventLoop
     * @brief 事件循环接口
     */
    class IEventLoop
    {
    public:
        /**
         * @brief 构造函数
         */
        IEventLoop() = default;
        /**
         * @brief 判断事件是否有效
         * @param type 事件类型
         * @return true 事件有效
         * @return false 事件无效
         */
        static bool is_event_valid(EventLoopEventType type);
        /**
         * @brief 运行事件循环
         */
        virtual void run() = 0;
        /**
         * @brief 添加套接字
         * @param socket 套接字
         * @param type 事件类型
         * @return true 添加成功
         * @return false 添加失败
         */
        virtual bool add_socket(std::unique_ptr<IClientSocket> socket, EventLoopEventType type) = 0;
        /**
         * @brief 移除套接字
         * @param socket_id socket逻辑标识符
         */
        virtual void remove_socket(int32_t socket_id) = 0;
        /**
         * @brief 判断事件循环是否有效
         * @return true 事件循环有效
         * @return false 事件循环无效
         */
        virtual bool is_valid() = 0;
        /**
         * @brief 停止事件循环
         */
        virtual void stop() = 0;
    protected:
    };
}


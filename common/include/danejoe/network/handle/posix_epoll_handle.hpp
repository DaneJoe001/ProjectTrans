/**
 * @file posix_epoll_handle.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief epoll 句柄封装
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 PosixEpollHandle，用于封装 Linux epoll 句柄的创建与控制操作。
 *          该文件仅在 Linux 平台启用（由 DANEJOE_PLATFORM_LINUX 宏控制）。
 */
#pragma once

#include "danejoe/common/result/result.hpp"
#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

#if DANEJOE_PLATFORM_LINUX==1

/// @brief epoll 事件结构体前置声明
struct epoll_event;

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @class PosixEpollHandle
     * @brief epoll 句柄封装
     * @details 以 RAII 方式管理 epoll 文件描述符，并提供 epoll_ctl/epoll_wait 的封装接口。
     *          - 资源管理：由 UniqueHandle<int> 持有 fd
     *          - 错误返回：
     *            - wait() 使用 Result 返回状态与 epoll_wait 返回值
     *            - add/modify/remove/control 使用 StatusCode 表达调用结果
     */
    class PosixEpollHandle
    {
    public:
        /**
         * @brief 默认构造
         */
        PosixEpollHandle();
        /**
         * @brief 使用指定 flags 构造并初始化
         * @param flags epoll_create1 的 flags
         */
        PosixEpollHandle(int flags);
        /**
         * @brief 句柄有效性判断
         * @return 若内部句柄有效则为 true，否则为 false
         */
        operator bool();
        /**
         * @brief 初始化 epoll 句柄
         * @param flags epoll_create1 的 flags
         */
        void init(int flags);
        /**
         * @brief 等待就绪事件
         * @param events 输出事件数组
         * @param max_events events 数组容量
         * @param time_out 超时（毫秒），语义与 epoll_wait 一致
         * @return 成功时返回 epoll_wait 的返回值（就绪事件数量），失败时通过 Result 返回错误状态
         */
        Result<int> wait(epoll_event* events, int max_events, int time_out);
        /**
         * @brief 监听 fd 并添加事件
         * @param watched_fd 被监听的文件描述符
         * @param event 事件描述（包含关注的事件与用户数据）
         * @return 操作结果状态码
         */
        StatusCode add(int watched_fd, epoll_event* event);
        /**
         * @brief 修改已监听 fd 的事件
         * @param watched_fd 被监听的文件描述符
         * @param event 新的事件描述
         * @return 操作结果状态码
         */
        StatusCode modify(int watched_fd, epoll_event* event);
        /**
         * @brief 移除已监听的 fd
         * @param watched_fd 被监听的文件描述符
         * @return 操作结果状态码
         */
        StatusCode remove(int watched_fd);
        /**
         * @brief 执行 epoll 控制操作
         * @param watched_fd 被监听的文件描述符
         * @param operation 控制操作（对应 epoll_ctl 的 op）
         * @param event 事件描述（视 operation 而定可为 nullptr）
         * @return 操作结果状态码
         */
        StatusCode control(
            int watched_fd,
            int operation,
            epoll_event* event);
        /**
         * @brief 获取底层句柄
         * @return 内部 UniqueHandle 引用
         */
        const UniqueHandle<int>& get_handle()const;
    private:
        /// @brief epoll 文件描述符句柄
        UniqueHandle<int> m_handle;
    };
};

#endif
/**
 * @file posix_event_handle.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief POSIX 事件句柄封装
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 PosixEventHandle，用于封装 Linux 平台下的通知事件句柄（通常用于唤醒 epoll_wait）。
 *          该文件仅在 Linux 平台启用（由 DANEJOE_PLATFORM_LINUX 宏控制）。
 */
#pragma once

#include <cstdint>

#include "danejoe/common/result/result.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

#if DANEJOE_PLATFORM_LINUX==1

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @class PosixEventHandle
     * @brief POSIX 通知事件句柄
     * @details 以 RAII 方式管理用于事件通知的文件描述符，并提供 read/write 的封装接口。
     *          典型用途：在事件循环线程阻塞等待时，由其它线程写入通知值以唤醒。
     */
    class PosixEventHandle
    {
    public:
        /**
         * @brief 默认构造
         */
        PosixEventHandle();
        /**
         * @brief 使用初始计数与 flags 构造并初始化
         * @param count 初始计数值
         * @param flags 创建 flags（语义与底层实现一致）
         */
        PosixEventHandle(uint32_t count, int flags);
        /**
         * @brief 句柄有效性判断
         * @return 若内部句柄有效则为 true，否则为 false
         */
        operator bool();
        /**
         * @brief 初始化事件句柄
         * @param count 初始计数值
         * @param flags 创建 flags（语义与底层实现一致）
         */
        void init(uint32_t count, int flags);
        /**
         * @brief 读取通知值
         * @return 成功时返回读取到的通知值；失败时通过 Result 返回错误状态
         */
        Result<uint64_t> read();
        /**
         * @brief 写入通知值
         * @param value 写入的通知值
         * @return 写入结果；失败时通过 Result 返回错误状态
         */
        Result<int> write(uint64_t value);
        /**
         * @brief 获取底层句柄
         * @return 内部 UniqueHandle 引用
         */
        const UniqueHandle<int>& get_handle()const;
    private:
        /// @brief 通知事件文件描述符句柄
        UniqueHandle<int> m_handle;
    };
}
#endif
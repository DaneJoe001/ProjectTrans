/**
 * @file connect_context.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 连接上下文
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 ConnectContext，用于封装单个连接的读写状态。
 *          该类将底层套接字句柄、帧组装器以及待发送缓冲区组合在一起，并使用 Result 返回读写结果。
 */
#pragma once

#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/codec/frame_assembler.hpp"
#include "danejoe/common/result/result.hpp"
#include "danejoe/network/container/posix_frame.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    /**
     * @class ConnectContext
     * @brief 连接上下文
     * @details 用于管理一个连接的读写过程：
     *          - read() 从 socket 读取字节并交由 FrameAssembler 组装为 PosixFrame
     *          - write() 将待发送帧写入 socket，并在必要时缓存未写完的字节
     * @note 线程安全：通常假设同一连接的 read/write 在同一线程或外部同步下调用。
     */
    class ConnectContext
    {
    public:
        /**
         * @brief 构造连接上下文
         * @param connect_id 连接标识
         * @param socket_handle 连接对应的套接字句柄（移动接管其所有权）
         */
        ConnectContext(
            uint64_t connect_id,
            PosixSocketHandle&& socket_handle);
        /**
         * @brief 读取并组装帧
         * @return 读取并成功组装得到的帧集合；失败时通过 Result 返回错误状态
         */
        Result<std::vector<PosixFrame>> read();
        /**
         * @brief 写入待发送帧
         * @param frames 待写入的帧集合
         * @return 写入结果；返回值通常表示本次写入的字节数或状态码，失败时通过 Result 返回错误状态
         */
        Result<int> write(std::vector<PosixFrame> frames);
        /**
         * @brief 是否存在待发送的缓存数据
         * @return 若存在未写完的数据则返回 true，否则返回 false
         */
        bool has_pending_write() const;
        /**
         * @brief 获取连接标识
         * @return 连接标识
         */
        uint64_t get_connect_id()const;
    private:
        /// @brief 每次从 socket 读取的缓冲区大小（字节）
        const int BUFFER_SIZE = 1024;
        /// @brief 连接标识
        uint64_t m_connect_id = 0;
        /// @brief 套接字句柄
        PosixSocketHandle m_socket_handle;
        /// @brief 帧组装器（将字节流组装为帧）
        FrameAssembler m_frame_assembler;
        /// @brief 待发送缓存（用于保存未写完的数据）
        Buffer m_write_buffer;
    };
#endif
}
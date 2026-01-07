/**
 * @file posix_socket_handle.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief POSIX socket 句柄封装
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 PosixSocketHandle，用于封装 Linux 平台下 socket 文件描述符与常用网络操作。
 *          该类以 RAII 方式管理 fd（由 UniqueHandle<int> 持有），并通过 Result/StatusCode 返回调用结果。
 */
#pragma once

#include <cstddef>

#include "danejoe/network/container/buffer.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/result/result.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <sys/socket.h>
#endif

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    /**
     * @class PosixSocketHandle
     * @brief POSIX socket 句柄封装
     * @details 封装 socket 的创建、连接、监听、accept 以及基本的读写操作。
     *          - read()/read_some()：读取字节并返回 Buffer
     *          - write()/write_some()：写入字节并返回写入字节数
     *          - set_blocking()：设置阻塞/非阻塞模式，并记录当前模式
     * @note 线程安全：通常假设同一 socket 的读写由外部保证同步。
     */
    class PosixSocketHandle
    {
    public:
        /**
         * @brief 默认构造
         */
        PosixSocketHandle();
        /**
         * @brief 创建 socket
         * @param domain 协议族（如 AF_INET）
         * @param type socket 类型（如 SOCK_STREAM）
         * @param protocol 协议号
         */
        PosixSocketHandle(int domain, int type, int protocol);
        /**
         * @brief 从已有 fd 构造
         * @param other 已有 socket 文件描述符
         */
        PosixSocketHandle(int other);
        /**
         * @brief 句柄有效性判断
         * @return 若内部句柄有效则为 true，否则为 false
         */
        operator bool();
        /**
         * @brief 读取指定长度的数据
         * @param size 期望读取的字节数
         * @return 读取到的数据；失败时通过 Result 返回错误状态
         */
        Result<Buffer> read(std::size_t size);
        /**
         * @brief 尝试读取部分数据
         * @param size 最大读取字节数
         * @return 读取到的数据（可能少于 size）；失败时通过 Result 返回错误状态
         */
        Result<Buffer> read_some(std::size_t size);
        /**
         * @brief 写入整个缓冲区
         * @param buffer 待写入数据
         * @return 写入字节数；失败时通过 Result 返回错误状态
         */
        Result<std::size_t> write(DaneJoe::Buffer buffer);
        /**
         * @brief 尝试写入部分缓冲区
         * @param buffer 待写入数据
         * @return 实际写入字节数；失败时通过 Result 返回错误状态
         */
        Result<std::size_t> write_some(DaneJoe::Buffer buffer);
        /**
         * @brief 尝试写入指定内存区域
         * @param buffer 待写入内存指针
         * @param size 待写入字节数
         * @return 实际写入字节数；失败时通过 Result 返回错误状态
         */
        Result<std::size_t> write_some(uint8_t* buffer, std::size_t size);
        /**
         * @brief 连接到远端地址
         * @param address 远端地址
         * @param length address 长度
         * @return 操作结果状态码
         */
        StatusCode connect(const sockaddr* address, socklen_t length);
        /**
         * @brief 绑定到本地地址
         * @param address 本地地址
         * @param length address 长度
         * @return 操作结果状态码
         */
        StatusCode bind(const sockaddr* address, socklen_t length);
        /**
         * @brief 开始监听
         * @param nums listen backlog
         * @return 操作结果状态码
         */
        StatusCode listen(int nums);
        /**
         * @brief 接受一个新连接
         * @param address 输出远端地址（可为 nullptr）
         * @param length 输入/输出 address 长度（可为 nullptr）
         * @param flags accept4 的 flags
         * @return 新连接对应的 socket 句柄；失败时通过 Result 返回错误状态
         */
        Result<PosixSocketHandle> accept(
            sockaddr* address = nullptr,
            socklen_t* length = nullptr,
            int flags = 0);
        /**
         * @brief 设置阻塞模式
         * @param is_blocking true 表示阻塞，false 表示非阻塞
         * @return 操作结果状态码
         */
        StatusCode set_blocking(bool is_blocking);
        /**
         * @brief 获取底层句柄
         * @return 内部 UniqueHandle 引用
         */
        const UniqueHandle<int>& get_handle()const;
    private:
        /// @brief 当前是否为阻塞模式
        bool m_is_blocking = false;
        /// @brief socket 文件描述符句柄
        UniqueHandle<int> m_handle;
    };
#endif
};
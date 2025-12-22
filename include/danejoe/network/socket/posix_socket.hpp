/**
 * @file posix_socket.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Posix套接字
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#if defined(__linux__)

#include <cstdint>

#include <sys/socket.h>

#include "danejoe/network/socket/i_socket.hpp"
#include "danejoe/network/socket/posix_socket_option.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{

    /**
     * @class PosixSocket
     * @brief Posix套接字
     */
    class PosixSocket :virtual public ISocket
    {
    public:
        /**
         * @brief 计算获取逻辑标识符
         * @param fd 原始socket句柄
         * @return 逻辑标识符
         * @todo 必须统一所有继承类，加入模板类型审查是否有该函数
         */
        static int32_t get_id(int32_t fd);
        /**
         * @brief 转换为POSIX选项
         * @param option 选项
         * @return Posix选项
         */
        static PosixSocketOption to_posix_option(const ISocketOption& option);
        /**
         * @brief 判断套接字是否有效
         * @return true 套接字有效
         * @return false 套接字无效
         */
        bool is_valid()const override;
        /**
         * @brief 设置选项
         * @param option 选项
         * @return true 设置成功
         * @return false 设置失败
         */
        bool set_opt(const ISocketOption& option)override;
        /**
         * @brief 关闭套接字
         */
        void close()override;
        /**
         * @brief 获取逻辑标识符
         * @return 逻辑标识符
         */
        int32_t get_id()const override;
        /**
         * @brief 设置非阻塞模式
         * @param status 是否为非阻塞模式
         * @return true 设置成功
         * @return false 设置失败
         */
        bool set_non_blocking(bool status)override;
        /**
         * @brief 获取原始套接字
         * @return 原始套接字句柄
         */
        const void* get_raw_socket()const override;
        /**
         * @brief 判断两个套接字是否相等
         * @param rhs 右操作数
         * @return true 两个套接字相等
         * @return false 两个套接字不相等
         */
        bool operator==(const PosixSocket& rhs)const;
        /**
         * @brief 判断两个套接字是否相等
         * @param socket_fd 套接字句柄
         * @return true 两个套接字相等
         * @return false 两个套接字不相等
         */
        bool operator==(int32_t socket_fd)const;
        /**
         * @brief 检查套接字是否可读
         * @return true 套接字可读
         * @return false 套接字不可读
         */
        bool is_readable()const override;
        /**
         * @brief 检查套接字是否可写
         * @return true 套接字可写
         * @return false 套接字不可写
         */
        bool is_writable()const override;
    protected:
        /// @brief 套接字句柄
        int32_t m_socket = -1;
        /// @brief 接收缓存大小
        uint32_t m_recv_buffer_size = 4096;
        /// @brief 接收块大小
        uint32_t m_recv_block_size = 1024;
    };
}
#endif

#pragma once

#include <sys/socket.h>

#include "common/network/i_socket.hpp"

/**
 * @class PosixSocket
 * @brief PosixSocket类实现了PosixSocket接口，用于表示POSIX套接字。
 */
class PosixSocket :virtual public ISocket {
public:
    /**
     * @struct PosixOption
     * @brief PosixOption类实现了PosixOption接口，用于表示POSIX选项。
     */
    struct PosixOption
    {
        /// @brief 选项级别
        int level;
        /// @brief 选项名称
        int opt_name;
        /// @brief 选项值
        void* opt_val;
        /// @brief 选项长度
        socklen_t opt_len;
    };
public:
    /**
     * @brief 计算获取逻辑标识符
     * @param fd 原始socket句柄
     * @todo 必须统一所有继承类，加入模板类型审查是否有该函数
     */
    static int get_id(int fd);
    /**
     * @brief 转换为POSIX选项
     * @param option 选项
     * @return POSIX选项
     */
    static PosixOption to_posix_option(const IOption& option);
    /**
     * @brief 判断套接字是否有效
     * @return 是否有效
     */
    bool is_valid()const override;
    /**
     * @brief 设置选项
     * @param option 选项
     * @return 是否成功
     */
    bool set_opt(const IOption& option)override;
    /**
     * @brief 关闭套接字
     */
    void close()override;
    /**
     * @brief 获取逻辑标识符
     * @return 逻辑标识符
     */
    int get_id()const override;
    /**
     * @brief 设置非阻塞模式
     * @param status 是否为非阻塞模式
     * @return 是否成功
     */
    bool set_non_blocking(bool status)override;
    /**
     * @brief 获取原始套接字
     * @return 原始套接字
     */
    const void* get_raw_socket()const override;
    /**
     * @brief 判断两个套接字是否相等
     * @param rhs 右操作数
     * @return 是否相等
     */
    bool operator==(const PosixSocket& rhs)const;
    /**
     * @brief 判断两个套接字是否相等
     * @param socket_fd 套接字句柄
     * @return 是否相等
     */
    bool operator==(int socket_fd)const;
    bool is_readable()const override;
    bool is_writeable()const override;
protected:
    /// @brief 套接字句柄
    int m_socket = -1;
    /// @brief 接收缓存大小
    std::size_t m_recv_buffer_size = 4096;
    /// @brief 接收块大小
    std::size_t m_recv_block_size = 1024;
};
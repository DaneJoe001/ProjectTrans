#pragma once

/**
 * @file i_socket.hpp
 * @brief 套接字接口
 * @author DaneJoe001
 */

#include <cstdint>
#include <vector>
#include <memory>

/**
 * @class ISocket
 * @brief ISocket类实现了ISocket接口，用于表示套接字。
 */
class ISocket
{
public:
    /**
     * @struct IOption
     * @brief 选项
     */
    struct IOption
    {
        /// @brief 选项级别
        int32_t level;
        /// @brief 选项名称
        int32_t opt_name;
        /// @brief 选项值
        void* opt_val = nullptr;
        /// @brief 选项长度
        int32_t opt_len;
    };
public:
    virtual ~ISocket() = default;
    /**
     * @brief 获取原始套接字
     * @return 原始套接字
     */
    virtual const void* get_raw_socket()const = 0;
    /**
     * @brief 判断套接字是否有效
     * @return 是否有效
     */
    virtual bool is_valid()const = 0;
    /**
     * @brief 获取逻辑标识符
     * @return 根据不同实现计算返回不同标识符
     */
    virtual int32_t get_id()const = 0;
    /**
     * @brief 设置选项
     * @param option 选项
     * @return 是否成功
     */
    virtual bool set_opt(const IOption& option) = 0;
    /**
     * @brief 设置非阻塞模式
     * @param status 是否为非阻塞模式
     * @return 是否成功
     */
    virtual bool set_non_blocking(bool status) = 0;
    /**
     * @brief 关闭套接字
     */
    virtual void close() = 0;
    /**
     * @brief 检查套接字是否可读
     * @return 是否可读
     */
    virtual bool is_readable()const = 0;
    /**
     * @brief 检查套接字是否可写
     * @return 是否可写
     */
    virtual bool is_writeable()const = 0;
protected:
    /// @brief 是否为非阻塞模式
    bool m_is_non_blocking = false;
};
#pragma once

/**
 * @file connection_manager.hpp
 * @brief 客户端连接管理分配
 * @author DaneJoe001
 */

#include <mutex>
#include <map>
#include <chrono>
#include <cstdint>

#include "client/connect/client_connection.hpp"

 /**
  * @class ConnectionGuard
  * @brief RAII封装连接类
  * @note RAII管理连接的生命周期
  */
class ConnectionGuard
{
public:
    /**
     * @brief 构造函数
     * @param connection 连接
     */
    ConnectionGuard(std::unique_ptr<ClientConnection> connection);
    /**
     * @brief 析构函数
     */
    ~ConnectionGuard();
    /**
     * @brief 判断是否连接
     * @return 是否连接
     */
    operator bool()const;
    /**
     * @brief 获取连接
     * @return 连接
     */
    ClientConnection* operator->();
    /**
     * @brief 获取连接
     * @return 连接
     */
    const ClientConnection* operator->()const;
    /**
     * @brief 移动赋值运算符
     * @param other 其他连接
     * @return 连接Guard
     */
    ConnectionGuard& operator=(ConnectionGuard&& other)noexcept;
    /**
     * @brief 移动构造函数
     * @param other 其他连接
     */
    ConnectionGuard(ConnectionGuard&& other)noexcept;
private:
    /**
     * @brief 复制构造函数
     * @param other 其他连接
     * @note 禁止复制构造
     */
    ConnectionGuard(const ConnectionGuard&) = delete;
    /**
     * @brief 复制赋值运算符
     * @param other 其他连接
     * @note 禁止复制赋值
     */
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;
private:
    /// @brief 连接
    std::unique_ptr<ClientConnection> m_connection;
};

/**
 * @class ConnectionManager
 * @brief 连接管理器
 */
class ConnectionManager
{
public:
    /**
     * @enum ConnectionStatus
     * @brief 连接状态枚举
     * @note 考虑未来的拓展和位运算
     */
    enum class ConnectionStatus
    {
        /// @brief 不可用
        Unavailable = 0,
        /// @brief 可用
        Available = 1 << 0,
    };
    /**
     * @struct ConnectionInfo
     * @brief 连接信息
     */
    struct ConnectionInfo
    {
        /// @brief 连接
        std::unique_ptr<ClientConnection> m_connection;
        /// @brief 连接状态
        ConnectionStatus m_status;
        /// @brief 上次使用时间
        std::chrono::time_point<std::chrono::system_clock> m_last_used_time;
    };
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
    static ConnectionManager& get_instance();
    /**
     * @brief 添加连接
     * @param ip 服务器IP地址
     * @param port 服务器端口
     */
    void add_connection(const std::string& ip, uint16_t port);
    /**
     * @brief 删除连接
     * @param ip 服务器IP地址
     * @param port 服务器端口
     */
    void remove_connection(const std::string& ip, uint16_t port);
    /**
     * @brief 获取连接
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 连接
     */
    std::unique_ptr<ClientConnection> get_connection(const std::string& ip, uint16_t port);
    /**
     * @brief 获取连接Guard
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 连接Guard
     */
    ConnectionGuard get_connection_guard(const std::string& ip, uint16_t port);
    /**
     * @brief 回收连接
     * @note 当管理器列表中键值存在时才进行回收
     * @note 对于外部直接创建的不做处理
     * @param connection 回收的连接
     */
    void recycle_connection(std::unique_ptr<ClientConnection> connection);
    /**
     * @brief 设置最大连接数
     * @param count 最大连接数
     */
    void set_max_connection_count(int32_t count);
    /**
     * @brief 获取连接数
     * @return 连接数
     */
    int32_t get_connection_count()const;
    /**
     * @brief 获取连接数
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 连接数
     */
    int32_t get_connection_count(const std::string& ip, uint16_t port)const;
    /**
     * @brief 清除未使用的连接
     */
    void clear_unused_connections();
private:
    /**
     * @brief 构造函数
     */
    ConnectionManager();
    /**
     * @brief 析构函数
     */
    ~ConnectionManager();
private:
    /// @brief 连接信息映射
    /// @note 考虑使用std::unordered_multimap但不必须
    std::multimap<std::pair<std::string, uint16_t>, ConnectionInfo> m_connection_info_map;
    /// @brief 连接互斥锁
    mutable std::mutex m_connection_mutex;
    /// @brief 连接数
    int32_t m_connection_count = 0;
    /// @brief 最大连接数
    int32_t m_max_connection_count = 10;
    /// @brief 最大相同IP端口连接数
    int32_t m_max_same_ip_port_count = 10;
};
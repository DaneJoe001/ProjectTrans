#pragma once

/**
 * @file connection_manager.hpp
 * @brief 客户端连接管理分配
 */

#include <mutex>
#include <map>
#include <chrono>
#include <cstdint>

#include "client/connect/client_connection.hpp"

class ConnectionGuard
{
public:
    ConnectionGuard(std::unique_ptr<ClientConnection> connection);
    ~ConnectionGuard();
    operator bool()const;
    ClientConnection* operator->();
    const ClientConnection* operator->()const;
    ConnectionGuard& operator=(ConnectionGuard&& other)noexcept;
    ConnectionGuard(ConnectionGuard&& other)noexcept;
private:
    ConnectionGuard(const ConnectionGuard&) = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;
private:
    std::unique_ptr<ClientConnection> m_connection;
};

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
        Unavailable = 0,
        Available = 1 << 0,
    };
    struct ConnectionInfo
    {
        std::unique_ptr<ClientConnection> m_connection;
        ConnectionStatus m_status;
        /// @brief 后续考虑引入超时机制
        std::chrono::time_point<std::chrono::steady_clock> m_last_used_time;
    };
public:
    static ConnectionManager& get_instance();
    void add_connection(const std::string& ip, uint16_t port);
    void remove_connection(const std::string& ip, uint16_t port);
    std::unique_ptr<ClientConnection> get_connection(const std::string& ip, uint16_t port);
    ConnectionGuard get_connection_guard(const std::string& ip, uint16_t port);
    /**
     * @brief 回收连接
     * @note 当管理器列表中键值存在时才进行回收
     * @note 对于外部直接创建的不做处理
     * @param connection 回收的连接
     */
    void recycle_connection(std::unique_ptr<ClientConnection> connection);
    void set_max_connection_count(int32_t count);
    int32_t get_connection_count()const;
    int32_t get_connection_count(const std::string& ip, uint16_t port)const;
    void clear_unused_connections();
private:
    ConnectionManager();
    ~ConnectionManager();
private:
    /// @note 考虑使用std::unordered_multimap但不必须
    std::multimap<std::pair<std::string, uint16_t>, ConnectionInfo> m_connection_info_map;
    mutable std::mutex m_connection_mutex;
    int32_t m_connection_count = 0;
    int32_t m_max_connection_count = 10;
    int32_t m_max_same_ip_port_count = 10;
};
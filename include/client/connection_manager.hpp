#pragma once

/**
 * @file connection_manager.hpp
 * @brief 客户端连接管理分配
 */

#include <mutex>
#include <unordered_map>
#include <map>

#include "client/client_connection.hpp"
#include "mt_queue/mt_queue.hpp"


class ConnectionGuard
{
public:
    ConnectionGuard(std::unique_ptr<ClientConnection> connection);
    ~ConnectionGuard();
    ClientConnection* operator->();
private:
    ConnectionGuard(const ConnectionGuard&) = delete;
    ConnectionGuard(ConnectionGuard&&) = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;
    ConnectionGuard& operator=(ConnectionGuard&&) = delete;
private:
    std::unique_ptr<ClientConnection> m_connection;
};

class ConnectionManager
{
public:
    enum class ConnectionStatus
    {
        Unavailable = 0,
        Available = 1 << 0,
    };
public:
    static ConnectionManager& get_instance();
    void add_connection(const std::string& ip, uint16_t port);
    void remove_connection(const std::string& ip, uint16_t port);
    std::unique_ptr<ClientConnection> get_connection(const std::string& ip, uint16_t port);
    /**
     * @brief 回收连接
     * @note 当管理器列表中键值存在时才进行回收
     * @note 对于外部直接创建的不做处理
     * @param connection 回收的连接
     */
    void recycle_connection(std::unique_ptr<ClientConnection> connection);
    void set_max_connection_count(int count);
    int get_connection_count();
    void clear_unused_connections();
private:
    ConnectionManager();
    ~ConnectionManager();
private:
    std::multimap<std::pair<std::string, uint16_t>, std::unique_ptr<ClientConnection>> m_connections;
    /// @brief 记录连接状态
    /// @tparam int 0: 不可用 1: 可用
    std::multimap<std::pair<std::string, uint16_t>, ConnectionStatus> m_connection_status;
    std::mutex m_connection_mutex;
    std::mutex m_max_count_mutex;
    int m_connection_count = 0;
    int m_max_connection_count = 10;
};
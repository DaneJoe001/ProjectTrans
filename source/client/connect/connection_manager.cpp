#include "log/manage_logger.hpp"
#include "client/connect/connection_manager.hpp"

ConnectionGuard::ConnectionGuard(std::unique_ptr<ClientConnection> connection)
    : m_connection(std::move(connection))
{
}

ConnectionGuard::operator bool()const
{
    return m_connection != nullptr;
}

ConnectionGuard::~ConnectionGuard()
{
    if (m_connection)
    {
        ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
    }
}
ConnectionGuard::ConnectionGuard(ConnectionGuard&& other)noexcept
{
    m_connection = std::move(other.m_connection);
    other.m_connection = nullptr;
}

ConnectionGuard& ConnectionGuard::operator=(ConnectionGuard&& other)noexcept
{
    if (this == &other)
    {
        return *this;
    }
    m_connection = std::move(other.m_connection);
    other.m_connection = nullptr;
    return *this;
}

ClientConnection* ConnectionGuard::operator->()
{
    return m_connection.get();
}

ConnectionManager::ConnectionManager() {}
ConnectionManager::~ConnectionManager() {}

ConnectionManager& ConnectionManager::get_instance()
{
    static ConnectionManager instance;
    return instance;
}
void ConnectionManager::add_connection(const std::string& ip, uint16_t port)
{
    /// @todo 添加同一ip和port的连接上限设置
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    if (m_connection_count >= m_max_connection_count || get_connection_count(ip, port) >= m_max_same_ip_port_count)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to add new connection: Max connection count reached!");
        return;
    }

    m_connection_info_map.emplace(std::make_pair(ip, port), ConnectionInfo{ std::make_unique<ClientConnection>(ip, port), ConnectionStatus::Available, std::chrono::steady_clock::now() });

    m_connection_count++;
}
void ConnectionManager::remove_connection(const std::string& ip, uint16_t port)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_id = std::make_pair(ip, port);
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    /// @note erase 一次
    for (auto it = connection_range.first; it != connection_range.second;)
    {
        it = m_connection_info_map.erase(it);
        m_connection_count--;
    }
}
std::unique_ptr<ClientConnection> ConnectionManager::get_connection(const std::string& ip, uint16_t port)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_id = std::make_pair(ip, port);
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    auto connection_it = connection_range.first;
    if (connection_range.first == connection_range.second)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to get connection:No connection found!");
        return nullptr;
    }

    for (; connection_it != connection_range.second;++connection_it)
    {
        if (connection_it->second.m_status == ConnectionStatus::Available)
        {
            connection_it->second.m_last_used_time = std::chrono::steady_clock::now();
            connection_it->second.m_status = ConnectionStatus::Unavailable;
            return std::move(connection_it->second.m_connection);
        }
    }
    DANEJOE_LOG_WARN("default", "Connection Manager", "Connection is already in use!");
    return nullptr;
}

void ConnectionManager::recycle_connection(std::unique_ptr<ClientConnection> connection)
{
    if (!connection)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Connection is nullptr!");
        return;
    }
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_id = connection->get_id();
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    auto connection_it = connection_range.first;

    if (connection_range.first == connection_range.second)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to recycle connection:No connection record found!");
        return;
    }

    for (; connection_it != connection_range.second; ++connection_it)
    {
        if (connection_it->second.m_status == ConnectionStatus::Unavailable)
        {
            connection_it->second.m_last_used_time = std::chrono::steady_clock::now();
            connection_it->second.m_status = ConnectionStatus::Available;
            connection_it->second.m_connection = std::move(connection);
            return;
        }
    }
    DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to recycle connection:Connection is not belong to this connection manager!");
}

void ConnectionManager::clear_unused_connections()
{
    /// @todo 完善超时机制
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_info_it = m_connection_info_map.begin();
    while (connection_info_it != m_connection_info_map.end())
    {
        if (connection_info_it->second.m_status == ConnectionStatus::Available)
        {
            connection_info_it = m_connection_info_map.erase(connection_info_it);
            m_connection_count--;
        }
        else
        {
            ++connection_info_it;
        }
    }
}

void ConnectionManager::set_max_connection_count(int count)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    m_max_connection_count = count;
}

int ConnectionManager::get_connection_count()const
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    return m_connection_count;
}

int ConnectionManager::get_connection_count(const std::string& ip, uint16_t port)const
{
    auto connection_id = std::make_pair(ip, port);
    return m_connection_info_map.count(connection_id);
}

ConnectionGuard ConnectionManager::get_connection_guard(const std::string& ip, uint16_t port)
{
    return ConnectionGuard(get_connection(ip, port));
}

const ClientConnection* ConnectionGuard::operator->()const
{
    return m_connection.get();
}
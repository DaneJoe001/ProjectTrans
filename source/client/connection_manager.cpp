#include "log/manage_logger.hpp"
#include "client/connection_manager.hpp"

ConnectionGuard::ConnectionGuard(std::unique_ptr<ClientConnection> connection)
    : m_connection(std::move(connection))
{
}
ConnectionGuard::~ConnectionGuard()
{
    ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
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
    std::scoped_lock<std::mutex, std::mutex> lock(m_max_count_mutex, m_connection_mutex);
    if (m_connection_count >= m_max_connection_count)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to create add new connectionr:Max connection count reached!");
        return;
    }
    m_connections.emplace(std::make_pair(ip, port), std::make_unique<ClientConnection>(ip, port));
    m_connection_status.emplace(std::make_pair(ip, port), 1);
    m_connection_count++;
}
void ConnectionManager::remove_connection(const std::string& ip, uint16_t port)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_range = m_connections.equal_range(std::make_pair(ip, port));
    auto connection_count = m_connections.count(std::make_pair(ip, port));
    /// @note erase 一次
    for (auto it = connection_range.first; it != connection_range.second;)
    {
        auto statu_it = m_connection_status.find(std::make_pair(ip, port));
        if (statu_it != m_connection_status.end())
        {
            m_connection_status.erase(statu_it);
        }
        it = m_connections.erase(it);
        m_connection_count--;
    }
}
std::unique_ptr<ClientConnection> ConnectionManager::get_connection(const std::string& ip, uint16_t port)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_id = std::make_pair(ip, port);
    auto status_range = m_connection_status.equal_range(connection_id);
    auto connection_range = m_connections.equal_range(connection_id);
    auto status_it = status_range.first;
    auto connection_it = connection_range.first;

    std::size_t count = m_connections.count(connection_id);
    if (count == 0)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to get connection:No connection found!");
        return nullptr;
    }

    for (; status_it != status_range.second && connection_it != connection_range.second; ++status_it, ++connection_it)
    {
        if (status_it->second == ConnectionStatus::Available)
        {
            status_it->second = ConnectionStatus::Unavailable;
            return std::move(connection_it->second);
        }
    }
    DANEJOE_LOG_WARN("default", "Connection Manager", "Connection is already in use!");
    return nullptr;
}

void ConnectionManager::recycle_connection(std::unique_ptr<ClientConnection> connection)
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto connection_id = connection->get_id();
    auto status_range = m_connection_status.equal_range(connection_id);
    auto connection_range = m_connections.equal_range(connection_id);
    auto status_it = status_range.first;
    auto connection_it = connection_range.first;

    std::size_t count = m_connections.count(connection_id);
    if (count == 0)
    {
        DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to recycle connection:No connection record found!");
        return;
    }

    for (; status_it != status_range.second && connection_it != connection_range.second; ++status_it, ++connection_it)
    {
        if (status_it->second == ConnectionStatus::Unavailable)
        {
            status_it->second = ConnectionStatus::Available;
            connection_it->second = std::move(connection);
            return;
        }
    }
    DANEJOE_LOG_WARN("default", "Connection Manager", "Failed to recycle connection:Connection is not belong to this connection manager!");
}

void ConnectionManager::clear_unused_connections()
{
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    auto status_it = m_connection_status.begin();
    while (status_it != m_connection_status.end())
    {
        if (status_it->second == ConnectionStatus::Unavailable)
        {
            /// @brief 找到对应的连接
            auto connection_it = m_connections.find(status_it->first);
            if (connection_it != m_connections.end())
            {
                m_connections.erase(connection_it);
            }
            /// @note erase 返回下一个迭代器
            status_it = m_connection_status.erase(status_it);
            m_connection_count--;
        }
        else
        {
            ++status_it;
        }
    }
}

void ConnectionManager::set_max_connection_count(int count)
{
    std::lock_guard<std::mutex> lock(m_max_count_mutex);
    m_max_connection_count = count;
}

int ConnectionManager::get_connection_count()
{
    return m_connection_count;
}
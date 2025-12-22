#include <danejoe/logger/logger_manager.hpp>

#include "client/connect/connection_manager.hpp"

ConnectionGuard::ConnectionGuard(std::unique_ptr<ClientConnection> connection)
    : m_connection(std::move(connection))
{}

ConnectionGuard::operator bool()const
{
    // 转化为判断指针是否为空
    return m_connection != nullptr;
}

ConnectionGuard::~ConnectionGuard()
{
    // 当内部链接有效时回收链接
    if (m_connection)
    {
        ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
    }
}
ConnectionGuard::ConnectionGuard(ConnectionGuard&& other)noexcept
{
    // 转移链接资源
    m_connection = std::move(other.m_connection);
    // 制空源链接
    other.m_connection = nullptr;
}

ConnectionGuard& ConnectionGuard::operator=(ConnectionGuard&& other)noexcept
{
    // 当源指向自身时，直接返回自身
    if (this == &other)
    {
        return *this;
    }
    // 转移链接资源
    m_connection = std::move(other.m_connection);
    // 制空源链接
    other.m_connection = nullptr;
    return *this;
}

ClientConnection* ConnectionGuard::operator->()
{
    // 返回原始指针
    return m_connection.get();
}

ConnectionManager::ConnectionManager() {}
ConnectionManager::~ConnectionManager() {}

ConnectionManager& ConnectionManager::get_instance()
{
    // 定义单例
    static ConnectionManager instance;
    // 返回单例
    return instance;
}
void ConnectionManager::add_connection(const std::string& ip, uint16_t port)
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 判断链接数量是否超出限制
    if (m_connection_count >= m_max_connection_count || get_connection_count(ip, port) >= m_max_same_ip_port_count)
    {
        DANEJOE_LOG_WARN("default", "ConnectionManager", "Failed to add new connection: Max connection count reached!");
        return;
    }
    // 在连接信息映射表中补充新的连接信息
    m_connection_info_map.emplace(std::make_pair(ip, port), ConnectionInfo{ std::make_unique<ClientConnection>(ip, port), ConnectionStatus::Available, std::chrono::system_clock::now() });
    // 更新总连接数
    m_connection_count++;
}
void ConnectionManager::remove_connection(const std::string& ip, uint16_t port)
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 生成连接ID
    auto connection_id = std::make_pair(ip, port);
    // 查找匹配的所有连接
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    // erase 返回的是下一个迭代器
    // 需要遍历才能完成清除所有对应连接
    for (auto it = connection_range.first; it != connection_range.second;)
    {
        it = m_connection_info_map.erase(it);
        // 更新当前连接数量
        m_connection_count--;
    }
}
std::unique_ptr<ClientConnection> ConnectionManager::get_connection(const std::string& ip, uint16_t port)
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 生成连接ID
    auto connection_id = std::make_pair(ip, port);
    // 查找匹配的所有连接
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    // 检查映射表中是否存在匹配的连接
    if (connection_range.first == connection_range.second)
    {
        DANEJOE_LOG_WARN("default", "ConnectionManager", "Failed to get connection:No connection found!");
        return nullptr;
    }
    // 获取匹配的首个迭代器
    auto connection_it = connection_range.first;
    for (; connection_it != connection_range.second;++connection_it)
    {
        // 检查连接状态是否可用
        if (connection_it->second.m_status == ConnectionStatus::Available)
        {
            // 更新使用时间
            connection_it->second.m_last_used_time = std::chrono::system_clock::now();
            // 更新可用状态
            connection_it->second.m_status = ConnectionStatus::Unavailable;
            // 转移连接
            return std::move(connection_it->second.m_connection);
        }
    }
    // 没有找到可用连接则返回空
    DANEJOE_LOG_WARN("default", "ConnectionManager", "Connection is already in use!");
    return nullptr;
}

void ConnectionManager::recycle_connection(std::unique_ptr<ClientConnection> connection)
{
    // 判断要收回的连接是否为空
    if (!connection)
    {
        DANEJOE_LOG_WARN("default", "ConnectionManager", "Connection is nullptr!");
        return;
    }
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 获取连接ID
    auto connection_id = connection->get_id();
    // 查找当前映射表中是否存在该连接记录
    auto connection_range = m_connection_info_map.equal_range(connection_id);
    // 获取首个迭代器
    auto connection_it = connection_range.first;
    // 判断是否有匹配的记录
    if (connection_range.first == connection_range.second)
    {
        DANEJOE_LOG_WARN("default", "ConnectionManager", "Failed to recycle connection:No connection record found!");
        return;
    }
    // 当记录存在时进行遍历
    for (; connection_it != connection_range.second; ++connection_it)
    {
        // 当含有空位时进行回收
        if (connection_it->second.m_status == ConnectionStatus::Unavailable)
        {
            // 更新使用时间
            connection_it->second.m_last_used_time = std::chrono::system_clock::now();
            // 更新连接可用状态
            connection_it->second.m_status = ConnectionStatus::Available;
            // 回收连接句柄
            connection_it->second.m_connection = std::move(connection);
            return;
        }
    }
    DANEJOE_LOG_WARN("default", "ConnectionManager", "Failed to recycle connection:Connection is not belong to this connection manager!");
}

void ConnectionManager::clear_unused_connections()
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 获取匹配的首个迭代器
    auto connection_info_it = m_connection_info_map.begin();
    // 遍历所有记录
    while (connection_info_it != m_connection_info_map.end())
    {
        // 当前采取的是，连接可用说明空闲可以移除
        /// @todo 后续改为使用超时机制判断
        if (connection_info_it->second.m_status == ConnectionStatus::Available)
        {
            // 移除并更新迭代器
            connection_info_it = m_connection_info_map.erase(connection_info_it);
            m_connection_count--;
        }
        else
        {
            // 更新迭代器
            ++connection_info_it;
        }
    }
}

void ConnectionManager::set_max_connection_count(int32_t count)
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 更新最大连接数
    m_max_connection_count = count;
}

int32_t ConnectionManager::get_connection_count()const
{
    // 对链接管理加锁
    std::lock_guard<std::mutex> lock(m_connection_mutex);
    // 返回当前连接数量
    return m_connection_count;
}

int32_t ConnectionManager::get_connection_count(const std::string& ip, uint16_t port)const
{
    // 生成连接id
    auto connection_id = std::make_pair(ip, port);
    // 返回匹配的连接数量
    return m_connection_info_map.count(connection_id);
}

ConnectionGuard ConnectionManager::get_connection_guard(const std::string& ip, uint16_t port)
{
    // 返回RAII版本的Connection
    return ConnectionGuard(get_connection(ip, port));
}

const ClientConnection* ConnectionGuard::operator->()const
{
    // 提供原始指针的访问
    return m_connection.get();
}
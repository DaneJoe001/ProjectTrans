#include <cstring>

#include "client/connect/client_connection.hpp"
#include "common/log/manage_logger.hpp"

ClientConnection::ClientConnection(const std::string& ip, uint16_t port) :m_ip(ip), m_port(port), m_socket(ip, port)
{
    // 设置套接字非阻塞模式
    m_socket.set_non_blocking(true);
}
ClientConnection::~ClientConnection()
{

}
void ClientConnection::send(const std::vector<uint8_t>& data)
{
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return;
    }
    // 发送数据
    m_socket.write_all(data);
}

std::vector<uint8_t> ClientConnection::receive()
{
    /// @todo 添加数据帧逻辑
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return std::vector<uint8_t>();
    }
    // 读取所有数据
    return m_socket.read_all();
}

bool ClientConnection::is_same(const std::string& ip, uint16_t port)
{
    // 判断ip和端口是否相同
    return m_ip == ip && m_port == port;
}

std::pair<std::string, uint16_t> ClientConnection::get_id()
{
    // 返回ip和端口作为ClientConnection的id
    return std::pair<std::string, uint16_t>(m_ip, m_port);
}
bool ClientConnection::is_connected()
{
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return false;
    }
    return m_socket.is_connected();
}

bool ClientConnection::is_readable()
{
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return false;
    }
    return m_socket.is_readable();
}
bool ClientConnection::is_writable()
{
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return false;
    }
    return m_socket.is_writable();
}
#include <cstring>

#include "client/connect/client_connection.hpp"
#include "log/manage_logger.hpp"

// #define ADD_LENGTH_INFO_TO_SEND
// #define ADD_LENGTH_INFO_TO_RECV

ClientConnection::ClientConnection(const std::string& ip, uint16_t port) :m_ip(ip), m_port(port), m_socket(ip, port)
{

}
ClientConnection::~ClientConnection()
{

}
void ClientConnection::send(const std::vector<uint8_t>& data)
{
    if (!m_socket.is_valid())
    {
        return;
    }
#ifdef ADD_LENGTH_INFO_TO_SEND
    std::vector<uint8_t> length(sizeof(data.size()));
    auto size = data.size();
    std::memcpy(length.data(), &size, sizeof(size));
    m_socket.send_all(length);
#endif
    m_socket.send_all(data);
}

std::vector<uint8_t> ClientConnection::recieve()
{
    if (!m_socket.is_valid())
    {
        return std::vector<uint8_t>();
    }
#ifdef ADD_LENGTH_INFO_TO_RECV
    /// @todo 分线程去接收
    auto length = m_socket.receive(sizeof(std::size_t));
    if (length.empty())
    {
        return std::vector<uint8_t>();
    }
    uint32_t size = 0;
    std::memcpy(&size, length.data(), sizeof(size));
    DANEJOE_LOG_TRACE("default", "ClientConnection", "recieve size:{}", size);
    return m_socket.receive(size);
#else
    return m_socket.read_all();
#endif
}

bool ClientConnection::is_same(const std::string& ip, uint16_t port)
{
    return m_ip == ip && m_port == port;
}

std::pair<std::string, uint16_t> ClientConnection::get_id()
{
    return std::pair<std::string, uint16_t>(m_ip, m_port);
}
bool ClientConnection::is_connected()
{
    return m_socket.is_connected();
}

bool ClientConnection::is_readable()
{
    return m_socket.is_readable();
}
bool ClientConnection::is_writeable()
{
    return m_socket.is_writeable();
}
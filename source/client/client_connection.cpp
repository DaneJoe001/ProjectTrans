#include "client/client_connection.hpp"

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
    m_socket.send_all(data);
}
std::vector<uint8_t> ClientConnection::recieve()
{
    if (!m_socket.is_valid())
    {
        return std::vector<uint8_t>();
    }
    return m_socket.read_all();
}

bool ClientConnection::is_same(const std::string& ip, uint16_t port)
{
    return m_ip == ip && m_port == port;
}

std::pair<std::string, uint16_t> ClientConnection::get_id()
{
    return std::pair<std::string, uint16_t>(m_ip, m_port);
}
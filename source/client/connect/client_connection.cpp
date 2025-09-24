#include <cstring>

#include "client/connect/client_connection.hpp"
#include "log/manage_logger.hpp"

/// @todo 修复添加长度后的接收问题
// #define ADD_LENGTH_INFO_TO_SEND
// #define ADD_LENGTH_INFO_TO_RECV

ClientConnection::ClientConnection(const std::string& ip, uint16_t port) :m_ip(ip), m_port(port), m_socket(ip, port)
{
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
#ifdef ADD_LENGTH_INFO_TO_SEND
    // length数据存储长度信息
    std::vector<uint8_t> length(sizeof(uint32_t));
    // 获取数据长度
    uint32_t size = data.size();
    // 存储长度信息
    std::memcpy(length.data(), &size, sizeof(size));
    // 当长度为0时不进行发送
    if (size == 0)
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "ClientConnection", "To socket: {} ,send data: {}, data size: {}", m_socket.get_id(), std::string(data.begin(), data.end()), size);
    // 发送长度信息
    m_socket.write_all(length);
    // 发送数据
    m_socket.write_all(data);
#else
    // 发送数据
    m_socket.write_all(data);
#endif
}

std::vector<uint8_t> ClientConnection::receive()
{
    // 判断socket是否有效
    if (!m_socket.is_valid())
    {
        return std::vector<uint8_t>();
    }
#ifdef ADD_LENGTH_INFO_TO_RECV
    // 获取要接收的数据长度
    auto length = m_socket.read(sizeof(uint32_t));
    // 检查是否接收到长度信息
    if (length.empty())
    {
        //  没有接收到长度信息，返回空数据
        return std::vector<uint8_t>();
    }
    // 创建size变量用于存储长度信息
    uint32_t size = 0;
    // 将长度信息复制到size变量中
    std::memcpy(&size, length.data(), sizeof(size));
    // 当content长度为0时，返回空数据
    if (size == 0)
    {
        return std::vector<uint8_t>();
    }
    DANEJOE_LOG_TRACE("default", "ClientConnection", "Recieve size:{}", size);

    // 读取指定长度数据
    return m_socket.read(size);
#else
    // 读取所有数据
    return m_socket.read_all();
#endif
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
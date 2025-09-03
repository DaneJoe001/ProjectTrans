#include <cstring>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "log/manage_logger.hpp"
#include "network/posix_client_socket.hpp"

PosixClientSocket::PosixClientSocket(int socket)
{
    if (socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to create socket: socket fd is invalid");
    }
    m_socket = socket;
}

PosixClientSocket::PosixClientSocket(const std::string& ip, uint16_t port)
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to create socket");
    }
    connect(ip, port);
}

void PosixClientSocket::connect(const std::string& ip, uint16_t port)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to connect socket: socket is not valid");
    }
    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret = ::connect(m_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to connect socket");
    }
}
PosixClientSocket::~PosixClientSocket()
{
    this->close();
}

void PosixClientSocket::close()
{
    if (m_socket > 0)
    {
        ::close(m_socket);
        DANEJOE_LOG_TRACE("default", "Socket", "Closed socket");
    }
}

std::vector<uint8_t> PosixClientSocket::recieve(std::size_t size)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to recieve: socket is not valid");
    }
    std::vector<uint8_t> buffer(size);
    int ret = ::recv(m_socket, buffer.data(), size, 0);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to recieve");
    }
    return buffer;
}

void PosixClientSocket::send(const std::vector<uint8_t>& data)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: socket is not valid");
    }
    int ret = ::send(m_socket, data.data(), data.size(), 0);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to send");
    }
}

bool PosixClientSocket::is_valid()const
{
    return m_socket > 0;
}

PosixClientSocket::PosixOption PosixClientSocket::to_posix_option(const IOption& option)
{
    PosixOption posix_option;
    posix_option.level = option.level;
    posix_option.opt_name = option.opt_name;
    posix_option.opt_val = option.opt_val;
    posix_option.opt_len = option.opt_len;
    return posix_option;
}

bool PosixClientSocket::set_opt(const IOption& option)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: socket is not valid");
        return false;
    }
    if (option.opt_val == nullptr)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to set option: option value is null");
        return false;
    }
    int ret = setsockopt(m_socket, option.level, option.opt_name, option.opt_val, option.opt_len);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to set option");
        return false;
    }
    return true;
}

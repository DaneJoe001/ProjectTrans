#include <cstring>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "log/manage_logger.hpp"
#include "common/network/posix_server_socket.hpp"
#include "common/network/posix_client_socket.hpp"

PosixServerSocket::PosixServerSocket(PosixServerSocket&& other)
{
    int32_t socket_fd = other.m_socket;
    other.m_socket = -1;
    m_socket = socket_fd;
}

PosixServerSocket& PosixServerSocket::operator=(PosixServerSocket&& other)
{
    int32_t socket_fd = other.m_socket;
    other.m_socket = -1;
    m_socket = socket_fd;
    return *this;
}

PosixServerSocket::PosixServerSocket(const std::string& ip, uint16_t port, const IOption& option)
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to create socket");
    }
    this->set_opt(option);
    this->bind(ip, port);
    this->listen();
}



PosixServerSocket::~PosixServerSocket()
{
    this->close();
}

bool PosixServerSocket::bind(const std::string& ip, uint16_t port)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Faild to bind:Socket is not valid");
        return false;
    }
    /// @brief 描述套接字的结构体
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int32_t ret = ::bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to bind socket");
        return false;
    }
    return true;
}

bool PosixServerSocket::listen()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to listen:Socket is not valid");
        return false;
    }
    int32_t ret = ::listen(m_socket, 5);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to listen socket");
        return false;
    }
    return true;
}

std::unique_ptr<IClientSocket> PosixServerSocket::accept()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to accept:Socket is not valid");
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int32_t client_socket = ::accept(m_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to accept socket");
        return nullptr;
    }
    return std::make_unique<PosixClientSocket>(client_socket);
}
#include <cstring>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "log/manage_logger.hpp"
#include "network/posix_server_socket.hpp"
#include "network/posix_client_socket.hpp"

PosixServerSocket::PosixOption PosixServerSocket::to_posix_option(const IOption& option)
{
    PosixOption posix_option;
    posix_option.level = option.level;
    posix_option.opt_name = option.opt_name;
    posix_option.opt_val = option.opt_val;
    posix_option.opt_len = option.opt_len;
    return posix_option;
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

void PosixServerSocket::close()
{
    if (m_socket > 0)
    {
        ::close(m_socket);
        DANEJOE_LOG_TRACE("default", "Socket", "Closed socket");
    }
}

PosixServerSocket::~PosixServerSocket()
{
    this->close();
}

bool PosixServerSocket::is_valid()const
{
    return m_socket > 0;
}

void PosixServerSocket::bind(const std::string& ip, uint16_t port)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Faild to bind:Socket is not valid");
    }
    /// @brief 描述套接字的结构体
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret = ::bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to bind socket");
    }
}

void PosixServerSocket::listen()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to listen:Socket is not valid");
    }
    int ret = ::listen(m_socket, 5);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to listen socket");
    }
}

std::unique_ptr<IClientSocket> PosixServerSocket::accept()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to accept:Socket is not valid");
    }
    m_socket = ::accept(m_socket, nullptr, nullptr);
    return std::make_unique<PosixClientSocket>(m_socket);
}

bool PosixServerSocket::set_opt(const IOption& option)
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
    DANEJOE_LOG_TRACE("default", "Socket", "Set option successfully");
    return true;
}
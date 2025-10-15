#include <cstring>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "common/log/manage_logger.hpp"
#include "common/network/posix_server_socket.hpp"
#include "common/network/posix_client_socket.hpp"

PosixServerSocket::PosixServerSocket(PosixServerSocket&& other)
{
    // 临时存储源socket
    int32_t socket_fd = other.m_socket;
    // 转移socket
    m_socket = socket_fd;
    // 制空socket
    other.m_socket = -1;
}

PosixServerSocket& PosixServerSocket::operator=(PosixServerSocket&& other)
{
    // 临时存储源socket
    int32_t socket_fd = other.m_socket;
    // 转移socket
    m_socket = socket_fd;
    // 制空socket
    other.m_socket = -1;
    return *this;
}

PosixServerSocket::PosixServerSocket(const std::string& ip, uint16_t port, const IOption& option)
{
    // 创建套接字
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to create socket");
    }
    // 设置套接字属性
    this->set_opt(option);
    // 绑定套接字
    this->bind(ip, port);
    // 监听套接字
    this->listen();
}

PosixServerSocket::~PosixServerSocket()
{
    // 关闭套接字
    this->close();
}

bool PosixServerSocket::bind(const std::string& ip, uint16_t port)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Faild to bind:Socket is not valid");
        return false;
    }
    /// @brief 描述套接字的结构体
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(port);
    addr.sin_addr.s_addr = ::inet_addr(ip.c_str());
    // 绑定套接字
    int32_t ret = ::bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to bind socket");
        return false;
    }
    return true;
}

bool PosixServerSocket::listen()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to listen:Socket is not valid");
        return false;
    }
    // 监听套接字
    int32_t ret = ::listen(m_socket, 5);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to listen socket");
        return false;
    }
    return true;
}

std::unique_ptr<IClientSocket> PosixServerSocket::accept()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to accept:Socket is not valid");
    }
    struct sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    // 接收连接
    int32_t client_socket = ::accept(m_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    // 检查接收结果，失败返回空
    if (client_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "PosixServerSocket", "Failed to accept socket");
        return nullptr;
    }
    return std::make_unique<PosixClientSocket>(client_socket);
}
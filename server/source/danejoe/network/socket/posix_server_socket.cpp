#if defined(__linux__)

#include <memory>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/socket/posix_server_socket.hpp"
#include "danejoe/network/socket/posix_client_socket.hpp"

DaneJoe::PosixServerSocket::PosixServerSocket(PosixServerSocket&& other)
{
    // 临时存储源socket
    int32_t socket_fd = other.m_socket;
    // 转移socket
    m_socket = socket_fd;
    // 制空socket
    other.m_socket = -1;
}

DaneJoe::PosixServerSocket& DaneJoe::PosixServerSocket::operator=(PosixServerSocket&& other)
{
    // 临时存储源socket
    int32_t socket_fd = other.m_socket;
    // 转移socket
    m_socket = socket_fd;
    // 制空socket
    other.m_socket = -1;
    return *this;
}

DaneJoe::PosixServerSocket::PosixServerSocket(const std::string& ip, uint16_t port, const ISocketOption& option)
{
    // 创建套接字
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        ADD_DIAG_ERROR("network", "Create server socket failed");
    }
    // 设置套接字属性
    this->set_opt(option);
    // 绑定套接字
    this->bind(ip, port);
    // 监听套接字
    this->listen();
}

DaneJoe::PosixServerSocket::~PosixServerSocket()
{
    // 关闭套接字
    this->close();
}

bool DaneJoe::PosixServerSocket::bind(const std::string& ip, uint16_t port)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Bind server socket failed: socket is not valid");
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
        ADD_DIAG_ERROR("network", "Bind server socket failed");
        return false;
    }
    return true;
}

bool DaneJoe::PosixServerSocket::listen()
{
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Listen failed: socket is not valid");
        return false;
    }
    // 监听套接字
    int32_t ret = ::listen(m_socket, 5);
    if (ret < 0)
    {
        ADD_DIAG_ERROR("network", "Listen failed");
        return false;
    }
    return true;
}

std::unique_ptr<DaneJoe::IClientSocket> DaneJoe::PosixServerSocket::accept()
{
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Accept client failed: socket is not valid");
    }
    struct sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    // 接收连接
    int32_t client_socket = ::accept(m_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    // 检查接收结果，失败返回空
    if (client_socket < 0)
    {
        ADD_DIAG_ERROR("network", "Accept client failed");
        return nullptr;
    }
    return std::make_unique<PosixClientSocket>(client_socket);
}

#endif

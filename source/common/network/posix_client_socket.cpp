#include <cstring>
#include <cerrno>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "log/manage_logger.hpp"
#include "common/network/posix_client_socket.hpp"

PosixClientSocket::PosixClientSocket(PosixClientSocket&& other)
{
    int socket_fd = other.m_socket;
    other.m_socket = -1;
    m_socket = socket_fd;
}

PosixClientSocket& PosixClientSocket::operator=(PosixClientSocket&& other)
{
    int socket_fd = other.m_socket;
    other.m_socket = -1;
    m_socket = socket_fd;
    return *this;
}

PosixClientSocket::PosixClientSocket(int socket)
{
    if (socket < 0)
    {
        DANEJOE_LOG_WARN("default", "Socket", "Create socket with invalid fd");
    }
    m_socket = socket;
}

PosixClientSocket::PosixClientSocket(const std::string& ip, uint16_t port)
{
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to create socket");
        return;
    }
    connect(ip, port);
}

bool PosixClientSocket::connect(const std::string& ip, uint16_t port)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to connect socket: socket is not valid");
        return false;
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
        return false;
    }
    return true;
}
PosixClientSocket::~PosixClientSocket()
{
    this->close();
}
std::vector<uint8_t> PosixClientSocket::receive(std::size_t size)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to recieve: socket is not valid");
    }
    std::size_t has_read = 0;
    std::vector<uint8_t> buffer(size);
    /// @brief 循环接收数据
    while (has_read < size)
    {
        int ret = ::recv(m_socket, buffer.data() + has_read, size - has_read, 0);
        if (ret < 0)
        {
            /// @param EINTR 函数被中断
            if (errno == EINTR)
            {
                continue;
            }
            /// @param EAGAIN 非阻塞模式下，没有数据可读或缓冲区没有空间可写
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return buffer;
            }
            else
            {
                DANEJOE_LOG_ERROR("default", "Socket", "Failed to recieve");
            }
        }
        else if (ret == 0)
        {
            DANEJOE_LOG_ERROR("default", "Socket", "Connection closed");
            close();
            return std::vector<uint8_t>();
        }
        else
        {
            has_read += ret;
        }
    }
    return buffer;
}

void PosixClientSocket::send_all(const std::vector<uint8_t>& data)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: socket is not valid");
        return;
    }
    std::size_t has_written = 0;
    while (has_written < data.size())
    {
        int ret = ::send(m_socket, data.data() + has_written, data.size() - has_written, 0);
        if (ret < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            else if (errno == ENOMEM)
            {
                DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: out of memory");
            }
            else if (errno == ECONNRESET)
            {
                close();
                DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: connection reset");
                return;
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: unknown error");
                return;
            }
        }
        else if (ret == 0)
        {
            close();
            DANEJOE_LOG_TRACE("default", "Socket", "Failed to send: connection closed");
            return;
        }
        else
        {
            has_written += ret;
        }
    }
}

std::vector<uint8_t> PosixClientSocket::read_all()
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to read_all: socket is not valid");
        return std::vector<uint8_t>();
    }
    int is_non_blocking = m_is_non_blocking;
    set_non_blocking(true);
    /// @brief 缓存区
    std::vector<uint8_t> buffer;
    buffer.reserve(m_recv_buffer_size);
    /// @brief 临时缓存区用于分段接收
    std::vector<uint8_t> temp(m_recv_block_size);
    while (true)
    {
        /// @todo 修复此处的异常
        int ret = ::read(m_socket, temp.data(), m_recv_block_size);
        if (ret < 0)
        {

            if (errno == EINTR)
            {
                continue;
            }
            /// @param EAGAIN 非阻塞模式下，没有数据可读或缓冲区没有空间可写
            /// @param EWOULDBLOCK EWOULDBLOCK 是 EAGAIN 的一种实现上的替代
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // DANEJOE_LOG_TRACE("default", "Socket", "No more data to read");
                break;
            }
            else
            {
                DANEJOE_LOG_ERROR("default", "Socket", "Failed to recieve: {}", strerror(errno));
                close();
                return buffer;
            }
        }
        else if (ret == 0)
        {
            close();
            DANEJOE_LOG_TRACE("default", "Socket", "Socket closed");
            break;
        }
        else if (ret > 0)
        {
            auto end = temp.begin() + ret;
            buffer.insert(buffer.end(), temp.begin(), end);
        }

    }
    set_non_blocking(is_non_blocking);
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
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to send: {}", strerror(errno));
    }
}
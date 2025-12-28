#if defined(__linux__)

#include <cstring>
#include <cerrno>

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/socket/posix_client_socket.hpp"

DaneJoe::PosixClientSocket::PosixClientSocket(PosixClientSocket&& other)
{
    // 临时存储socket句柄
    int32_t socket_fd = other.m_socket;
    // 设置当前句柄
    m_socket = socket_fd;
    // 转移完成后置空源对象句柄
    other.m_socket = -1;
}

DaneJoe::PosixClientSocket& DaneJoe::PosixClientSocket::operator=(PosixClientSocket&& other)
{
    // 临时存储socket句柄
    int32_t socket_fd = other.m_socket;
    // 设置当前句柄
    m_socket = socket_fd;
    // 转移完成后置空源对象句柄
    other.m_socket = -1;
    return *this;
}

DaneJoe::PosixClientSocket::PosixClientSocket(int32_t socket)
{
    // 检查socket是否有效，无效警告
    if (socket < 0)
    {
        ADD_DIAG_WARN("network", "Create client socket skipped: invalid fd");
    }
    m_socket = socket;
}

DaneJoe::PosixClientSocket::PosixClientSocket(const std::string& ip, uint16_t port)
{
    // 默认创建IPv4的TCP套接字
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        ADD_DIAG_ERROR("network", "Create client socket failed");
        return;
    }
    // 连接服务器
    connect(ip, port);
}

bool DaneJoe::PosixClientSocket::connect(const std::string& ip, uint16_t port)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Connect failed: socket is not valid");
        return false;
    }
    // 创建服务器地址结构体
    struct sockaddr_in server_address = {};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());
    // 连接服务器
    int32_t ret = ::connect(m_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    // 检查连接结果
    if (ret < 0)
    {
        ADD_DIAG_ERROR("network", "Connect failed");
        m_is_connected = false;
        return false;
    }
    // 更新连接状态
    m_is_connected = true;
    return true;
}

DaneJoe::PosixClientSocket::~PosixClientSocket()
{
    // 关闭套接字
    this->close();
}

std::vector<uint8_t> DaneJoe::PosixClientSocket::read(uint32_t size)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Receive failed: socket is not valid");
        return std::vector<uint8_t>();
    }
    // 检查连接状态
    if (!m_is_connected)
    {
        ADD_DIAG_ERROR("network", "Receive failed: socket is not connected");
        return std::vector<uint8_t>();
    }
    // 记录已经读取的字节数
    uint32_t has_read = 0;
    // 创建数据接收容器
    std::vector<uint8_t> buffer(size);
    // 循环接收数据
    while (has_read < size)
    {
        // 调用系统API接收数据
        int32_t ret = ::recv(m_socket, buffer.data() + has_read, size - has_read, 0);
        if (ret < 0)
        {
            // EINTR 函数被中断
            if (errno == EINTR)
            {
                continue;
            }
            // EAGAIN 非阻塞模式下，没有数据可读或缓冲区没有空间可写
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            // 其他错误，结束循环
            else
            {
                ADD_DIAG_ERROR("network", "Receive failed");
                break;
            }
        }
        // 判断对端是否被关闭
        else if (ret == 0)
        {
            ADD_DIAG_ERROR("network", "Receive failed: connection closed");
            close();
            break;
        }
        // 成功接收
        else
        {
            // 更新已读字节数
            has_read += ret;
        }
    }
    buffer.resize(has_read);
    return buffer;
}

void DaneJoe::PosixClientSocket::write_all(const std::vector<uint8_t>& data)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Send failed: socket is not valid");
        return;
    }
    // 检查连接状态
    if (!m_is_connected)
    {
        ADD_DIAG_ERROR("network", "Send failed: socket is not connected");
        return;
    }
    // 记录已发送字节数
    uint32_t has_written = 0;
    // 循环发送直至完成
    while (has_written < data.size())
    {
        // 调用系统函数发送数据
        int32_t ret = ::send(m_socket, data.data() + has_written, data.size() - has_written, 0);
        // 检查是否发送失败
        if (ret < 0)
        {
            // 意外中断进入下一次循环发送
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            // 检查是否写入溢出
            else if (errno == ENOMEM)
            {
                ADD_DIAG_ERROR("network", "Send failed: out of memory");
            }
            // 检查是否被信号中断
            else if (errno == EINTR)
            {
                continue;
            }
            // 检查是否连接已重置
            else if (errno == ECONNRESET)
            {
                close();
                ADD_DIAG_ERROR("network", "Send failed: connection reset");
                return;
            }
            else
            {
                ADD_DIAG_ERROR("network", "Send failed: unknown error");
                return;
            }
        }
        // 检查对端是否关闭
        else if (ret == 0)
        {
            // 关闭客户端
            close();
            ADD_DIAG_TRACE("network", "Send stopped: connection closed");
            return;
        }
        // 发送成功
        else
        {
            // 更新已发送字节数
            has_written += ret;
        }
    }
}

std::vector<uint8_t> DaneJoe::PosixClientSocket::read_all()
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Read all failed: socket is not valid");
        return std::vector<uint8_t>();
    }
    // 检查连接状态
    if (!m_is_connected)
    {
        ADD_DIAG_ERROR("network", "Read all failed: socket is not connected");
        return std::vector<uint8_t>();
    }
    // 缓存区
    std::vector<uint8_t> buffer;
    // 分配缓冲区空间
    buffer.reserve(m_recv_buffer_size);
    // 临时缓存区用于分段接收
    std::vector<uint8_t> temp(m_recv_block_size);
    while (true)
    {
        // 调用系统函数读取数据
        int32_t ret = ::read(m_socket, temp.data(), m_recv_block_size);
        // 检查是否读取失败
        if (ret < 0)
        {
            // 意外中断重新接收
            if (errno == EINTR)
            {
                continue;
            }
            // 非阻塞模式下，没有数据可读或缓冲区没有空间可写
            // EWOULDBLOCK EWOULDBLOCK 是 EAGAIN 的一种实现上的替代
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            // 其他错误
            else
            {
                ADD_DIAG_ERROR("network", "Read all failed: {}", std::strerror(errno));
                close();
                return buffer;
            }
        }
        // 检查对端是否关闭
        else if (ret == 0)
        {
            // 关闭套接字
            close();
            ADD_DIAG_TRACE("network", "Read all stopped: socket closed");
            break;
        }
        // 接收成功
        else if (ret > 0)
        {
            // 更新接收缓冲区迭代器
            auto end = temp.begin() + ret;
            // 写入数据
            buffer.insert(buffer.end(), temp.begin(), end);
        }
    }
    return buffer;
}

void DaneJoe::PosixClientSocket::write(const std::vector<uint8_t>& data)
{
    // 检查套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Send failed: socket is not valid");
        return;
    }
    // 检查连接状态
    if (!m_is_connected)
    {
        ADD_DIAG_ERROR("network", "Send failed: socket is not connected");
        return;
    }
    // 调用系统函数发送数据
    int32_t ret = ::send(m_socket, data.data(), data.size(), 0);
    // 检查发送结果
    if (ret < 0)
    {
        ADD_DIAG_ERROR("network", "Send failed: {}", std::strerror(errno));
    }
    else if (ret == 0)
    {
        // 关闭客户端
        close();
        ADD_DIAG_TRACE("network", "Send stopped: connection closed");
    }
}

bool DaneJoe::PosixClientSocket::is_connected()
{
    // 返回是否连接
    return m_is_connected;
}

#endif

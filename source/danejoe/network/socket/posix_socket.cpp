#if defined(__linux__)

extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
}

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/socket/posix_socket.hpp"
#include "danejoe/network/monitor/poll_socket_monitor.hpp"

int32_t DaneJoe::PosixSocket::get_id(int32_t fd)
{
    // 返回原始套接字描述符
    return fd;
}

DaneJoe::PosixSocketOption DaneJoe::PosixSocket::to_posix_option(const ISocketOption& option)
{
    // 按照标准转换
    PosixSocketOption posix_option;
    posix_option.level = option.level;
    posix_option.opt_name = option.opt_name;
    posix_option.opt_val = option.opt_value;
    posix_option.opt_len = option.opt_length;
    return posix_option;
}

void DaneJoe::PosixSocket::close()
{
    // 关闭套接字
    if (m_socket >= 0)
    {
        ::close(m_socket);
        m_socket = -1;
        ADD_DIAG_TRACE("network", "Close socket succeeded");
    }
}

bool DaneJoe::PosixSocket::is_valid()const
{
    // 检查套接字是否有效
    return m_socket >= 0;
}

bool DaneJoe::PosixSocket::set_opt(const ISocketOption& option)
{
    // 判断套接字是否有效
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Set socket option failed: socket is not valid");
        return false;
    }
    // 判断选项是否为空
    if (option.opt_value == nullptr)
    {
        ADD_DIAG_ERROR("network", "Set socket option failed: option value is null");
        return false;
    }
    // 设置套接字选项
    int32_t ret = ::setsockopt(m_socket, option.level, option.opt_name, option.opt_value, option.opt_length);
    // 检查设置结果
    if (ret < 0)
    {
        ADD_DIAG_ERROR("network", "Set socket option failed");
        return false;
    }
    ADD_DIAG_TRACE("network", "Set socket option succeeded");
    return true;
}

bool DaneJoe::PosixSocket::set_non_blocking(bool status)
{
    if (!is_valid())
    {
        ADD_DIAG_ERROR("network", "Set non blocking failed: socket is not valid");
        return false;
    }
    if (status)
    {
        if (m_is_non_blocking)
        {
            return true;
        }
        int32_t flags = ::fcntl(m_socket, F_GETFL, 0);
        if (flags < 0)
        {
            ADD_DIAG_ERROR("network", "Set non blocking failed: get flags failed");
            return false;
        }
        flags = ::fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
        if (flags < 0)
        {
            ADD_DIAG_ERROR("network", "Set non blocking failed: set flags failed");
            return false;
        }
    }
    else
    {
        if (!m_is_non_blocking)
        {
            return true;
        }
        int32_t flags = ::fcntl(m_socket, F_GETFL, 0);
        if (flags < 0)
        {
            ADD_DIAG_ERROR("network", "Set non blocking failed: get flags failed");
            return false;
        }
        flags = ::fcntl(m_socket, F_SETFL, flags & ~O_NONBLOCK);
        if (flags < 0)
        {
            ADD_DIAG_ERROR("network", "Set non blocking failed: set flags failed");
            return false;
        }
    }
    m_is_non_blocking = status;
    return true;
}

int32_t DaneJoe::PosixSocket::get_id()const
{
    return m_socket;
}

const void* DaneJoe::PosixSocket::get_raw_socket()const
{
    return (const void*)&m_socket;
}

bool DaneJoe::PosixSocket::operator==(const PosixSocket& rhs)const
{
    return m_socket == rhs.m_socket;
}

bool DaneJoe::PosixSocket::operator==(int32_t socket_fd)const
{
    return m_socket == socket_fd;
}

bool DaneJoe::PosixSocket::is_readable()const
{
    return PollSocketMonitor::get_instance()->is_readable(this);
}
bool DaneJoe::PosixSocket::is_writable()const
{
    return PollSocketMonitor::get_instance()->is_writable(this);
}

#endif

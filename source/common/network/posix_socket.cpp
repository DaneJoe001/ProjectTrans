extern "C"
{
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
}

#include "log/manage_logger.hpp"
#include "common/network/posix_socket.hpp"

int PosixSocket::get_id(int fd)
{
    /// @brief 返回原始套接字描述符
    return fd;
}

PosixSocket::PosixOption PosixSocket::to_posix_option(const IOption& option)
{
    PosixOption posix_option;
    posix_option.level = option.level;
    posix_option.opt_name = option.opt_name;
    posix_option.opt_val = option.opt_val;
    posix_option.opt_len = option.opt_len;
    return posix_option;
}

void PosixSocket::close()
{
    if (m_socket >= 0)
    {
        ::close(m_socket);
        m_socket = -1;
        DANEJOE_LOG_TRACE("default", "Socket", "Closed socket");
    }
}

bool PosixSocket::is_valid()const
{
    return m_socket >= 0;
}

bool PosixSocket::set_opt(const IOption& option)
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
    int ret = ::setsockopt(m_socket, option.level, option.opt_name, option.opt_val, option.opt_len);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to set option");
        return false;
    }
    DANEJOE_LOG_TRACE("default", "Socket", "Set option successfully");
    return true;
}

bool PosixSocket::set_non_blocking(bool status)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Socket", "Failed to set non blocking: socket is not valid");
        return false;
    }
    if (status)
    {
        if (m_is_non_blocking)
        {
            return true;
        }
        int flags = ::fcntl(m_socket, F_GETFL, 0);
        if (flags < 0)
        {
            DANEJOE_LOG_ERROR("default", "Socket", "Failed to get flags");
            return false;
        }
        flags = ::fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
        if (flags < 0)
        {
            DANEJOE_LOG_ERROR("default", "Socket", "Failed to get flags");
            return false;
        }
    }
    else
    {
        if (!m_is_non_blocking)
        {
            return true;
        }
        int flags = ::fcntl(m_socket, F_GETFL, 0);
        if (flags < 0)
        {
            DANEJOE_LOG_ERROR("default", "Socket", "Failed to get flags");
            return false;
        }
        flags = ::fcntl(m_socket, F_SETFL, flags & ~O_NONBLOCK);
        if (flags < 0)
        {
            DANEJOE_LOG_ERROR("default", "Socket", "Failed to get flags");
            return false;
        }
    }
    m_is_non_blocking = status;
    return true;
}

int PosixSocket::get_id()const
{
    return m_socket;
}

const void* PosixSocket::get_raw_socket()const
{
    return (const void*)&m_socket;
}

bool PosixSocket::operator==(const PosixSocket& rhs)const
{
    return m_socket == rhs.m_socket;
}

bool PosixSocket::operator==(int socket_fd)const
{
    return m_socket == socket_fd;
}
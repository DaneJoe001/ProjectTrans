#include <cstring>

#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/status/posix_status_code.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>
#endif

#if DANEJOE_PLATFORM_LINUX==1

DaneJoe::PosixSocketHandle::PosixSocketHandle() {}

DaneJoe::PosixSocketHandle::PosixSocketHandle(int domain, int type, int protocol)
{
    m_handle = ::socket(domain, type, protocol);
}
DaneJoe::PosixSocketHandle::PosixSocketHandle(int other) :m_handle(other) {}

DaneJoe::PosixSocketHandle::operator bool()
{
    if (m_handle)
    {
        return true;
    }
    return false;
}

DaneJoe::Result<DaneJoe::Buffer> DaneJoe::PosixSocketHandle::read(std::size_t size)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return Result<Buffer>(status_code);
    }
    std::vector<uint8_t> buffer(size);
    std::size_t has_read = 0;
    std::size_t rest_size = size;
    while (has_read < size)
    {
        auto some_ret = read_some(rest_size);
        auto status_level = some_ret.status_code().get_status_level();
        if (status_level == StatusLevel::Ok)
        {
            auto read_size = some_ret.value().size();
            if (read_size == 0)
            {
                buffer.resize(has_read);
                return Result<Buffer>(buffer, make_posix_status_code(StatusLevel::Ok, "Peer closed!"));
            }
            std::memcpy(buffer.data() + has_read, some_ret.value().data(), read_size);
            has_read += read_size;
            rest_size -= read_size;
        }
        else if (status_level == StatusLevel::Branch)
        {

            buffer.resize(has_read);
            return Result<Buffer>(buffer, some_ret.status_code());
        }
        else
        {
            return Result<Buffer>(std::nullopt, some_ret.status_code());
        }
    }
    buffer.resize(has_read);
    return Result<Buffer>(buffer, make_posix_status_code(StatusLevel::Ok, "Read completely!"));
}
DaneJoe::Result<DaneJoe::Buffer>
DaneJoe::PosixSocketHandle::read_some(std::size_t size)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return Result<Buffer>(status_code);
    }
    std::vector<uint8_t> buffer(size);
    while (true)
    {
        int ret = ::read(m_handle.get(), buffer.data(), size);
        /// read_size>0 OK
        /// read_size==0 OK Peer closed
        /// read_size<0 Branch EINTER 在当前层重试
        /// read_size<0 Branch EAGAIN/EWOULDBLOCK 在上层考虑重试
        /// read_size<0 Error 错误状态
        if (ret > 0)
        {
            buffer.resize(ret);
            return Result<Buffer>(buffer, make_posix_status_code(StatusLevel::Ok));
        }
        else if (ret < 0)
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return Result<Buffer>(std::nullopt, status_code);
            }
        }
        else
        {
            buffer.resize(ret);
            return Result<Buffer>(buffer, make_posix_status_code(StatusLevel::Ok, "Peer closed!"));
        }
    }
}
DaneJoe::Result<std::size_t> DaneJoe::PosixSocketHandle::write(DaneJoe::Buffer buffer)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return Result<std::size_t>(status_code);
    }
    std::size_t has_write = 0;
    std::size_t rest_size = buffer.size();
    while (has_write < buffer.size())
    {
        auto some_ret = write_some(buffer.data() + has_write, rest_size);
        if (some_ret.status_code().get_status_level() == StatusLevel::Ok)
        {
            has_write += some_ret.value();
            rest_size -= some_ret.value();
        }
        else if (some_ret.status_code().get_status_level() == StatusLevel::Branch)
        {
            return Result<std::size_t>(has_write, some_ret.status_code());
        }
        else
        {
            return Result<std::size_t>(has_write, some_ret.status_code());
        }
    }
    return Result<std::size_t>(has_write, make_posix_status_code(StatusLevel::Ok));
}
DaneJoe::Result<std::size_t> DaneJoe::PosixSocketHandle::write_some(DaneJoe::Buffer buffer)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return Result<std::size_t>(status_code);
    }
    return write_some(buffer.data(), buffer.size());
}
DaneJoe::Result<std::size_t> DaneJoe::PosixSocketHandle::write_some(uint8_t* buffer, std::size_t size)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return Result<std::size_t>(status_code);
    }
    while (true)
    {
        ssize_t ret = ::write(m_handle.get(), buffer, size);
        if (ret > 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<std::size_t>(static_cast<std::size_t>(ret), status_code);
        }
        else if (ret == 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<std::size_t>(static_cast<std::size_t>(0), status_code);
        }
        else
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return Result<std::size_t>(std::nullopt, status_code);
            }
        }
    }
}
DaneJoe::StatusCode DaneJoe::PosixSocketHandle::connect(const sockaddr* address, socklen_t length)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return status_code;
    }
    while (true)
    {
        int ret = ::connect(m_handle.get(), address, length);
        if (ret == 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return status_code;
        }
        else if (ret == -1)
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return status_code;
            }
        }
        else
        {
            auto status_code = make_posix_status_code(StatusLevel::Error, "Unknown Error");
        }
    }
}
DaneJoe::StatusCode DaneJoe::PosixSocketHandle::bind(const sockaddr* address, socklen_t length)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return status_code;
    }
    while (true)
    {
        int ret = ::bind(m_handle.get(), address, length);
        if (ret == 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return status_code;
        }
        else if (ret == -1)
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return status_code;
            }
        }
        else
        {
            auto status_code = make_posix_status_code(StatusLevel::Error, "Unknown Error");
        }
    }
}
DaneJoe::StatusCode DaneJoe::PosixSocketHandle::listen(int nums)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return status_code;
    }
    while (true)
    {
        int ret = ::listen(m_handle.get(), nums);
        if (ret == 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return status_code;
        }
        else if (ret == -1)
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return status_code;
            }
        }
        else
        {
            auto status_code = make_posix_status_code(StatusLevel::Error, "Unknown Error");
        }
    }
}
DaneJoe::Result<DaneJoe::PosixSocketHandle> DaneJoe::PosixSocketHandle::accept(sockaddr* address, socklen_t* length, int flags)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return status_code;
    }
    while (true)
    {
        int ret = ::accept4(m_handle.get(), address, length, flags);
        if (ret >= 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<PosixSocketHandle>(PosixSocketHandle(ret), status_code);
        }
        else
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return Result<PosixSocketHandle>(status_code);
            }
        }
    }
}
DaneJoe::StatusCode DaneJoe::PosixSocketHandle::set_blocking(bool is_blocking)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(false, "Socket handle is invalid");
        return status_code;
    }
    int flags = ::fcntl(m_handle.get(), F_GETFL, 0);
    if (flags < 0)
    {
        auto status_code = make_posix_status_code();
        return status_code;
    }

    const bool current_blocking = ((flags & O_NONBLOCK) == 0);
    if (current_blocking == is_blocking)
    {
        m_is_blocking = is_blocking;
        return make_posix_status_code(StatusLevel::Ok);
    }
    if (is_blocking)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }
    flags = ::fcntl(m_handle.get(), F_SETFL, flags);
    if (flags < 0)
    {
        auto status_code = make_posix_status_code();
        return status_code;
    }
    m_is_blocking = is_blocking;
    return make_posix_status_code(StatusLevel::Ok);
}
const DaneJoe::UniqueHandle<int>& DaneJoe::PosixSocketHandle::get_handle()const
{
    return m_handle;
}
#endif
#include "danejoe/network/handle/posix_event_handle.hpp"
#include "danejoe/common/status/status_code.hpp"
#include "danejoe/network/status/posix_status_code.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <sys/eventfd.h>
#include <unistd.h>
DaneJoe::PosixEventHandle::PosixEventHandle() :m_handle(-1) {}
void DaneJoe::PosixEventHandle::init(uint32_t count, int flags)
{
    m_handle = ::eventfd(count, flags);
}
DaneJoe::PosixEventHandle::PosixEventHandle(uint32_t count, int flags)
{
    m_handle = ::eventfd(count, flags);
}
DaneJoe::PosixEventHandle::operator bool()
{
    if (m_handle)
    {
        return true;
    }
    return false;
}
DaneJoe::Result<uint64_t> DaneJoe::PosixEventHandle::read()
{
    if (!m_handle)
    {
        StatusCode status_code = make_posix_status_code(StatusLevel::Error, "Invalid event fd");
        return Result<uint64_t>(status_code);
    }
    uint64_t value = 0;
    while (true)
    {
        const ssize_t ret = ::read(m_handle.get(), reinterpret_cast<void*>(&value), sizeof(value));
        if (ret == static_cast<ssize_t>(sizeof(value)))
        {
            StatusCode status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<uint64_t>(value, status_code);
        }
        else if (ret < 0)
        {
            StatusCode status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else if (status_code == make_posix_status_code(EAGAIN) || status_code == make_posix_status_code(EWOULDBLOCK))
            {
                return Result<uint64_t>(static_cast<uint64_t>(0), status_code);
            }
            else
            {
                return Result<uint64_t>(std::nullopt, status_code);
            }
        }
        else
        {
            StatusCode status_code = make_posix_status_code(StatusLevel::Error, "Invalid eventfd read size");
            return Result<uint64_t>(std::nullopt, status_code);
        }
    }
}
DaneJoe::Result<int> DaneJoe::PosixEventHandle::write(uint64_t value)
{
    if (!m_handle)
    {
        StatusCode status_code = make_posix_status_code(StatusLevel::Error, "Invalid event fd!");
        return Result<int>(status_code);
    }
    if (value == 0)
    {
        StatusCode status_code = make_posix_status_code(StatusLevel::Error, "Invalid value!");
        return Result<int>(status_code);
    }
    while (true)
    {
        const ssize_t ret = ::write(m_handle.get(), reinterpret_cast<const void*>(&value), sizeof(value));
        if (ret == static_cast<ssize_t>(sizeof(value)))
        {
            StatusCode status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<int>(static_cast<int>(ret), status_code);
        }
        else if (ret < 0)
        {
            StatusCode status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else if (status_code ==
                make_posix_status_code(EAGAIN) ||
                status_code ==
                make_posix_status_code(EWOULDBLOCK))
            {
                return Result<int>(0, status_code);
            }
            else
            {
                return Result<int>(std::nullopt, status_code);
            }
        }
        else
        {
            StatusCode status_code = make_posix_status_code(StatusLevel::Error, "Invalid eventfd write size");
            return Result<int>(std::nullopt, status_code);
        }
    }
}
const DaneJoe::UniqueHandle<int>& DaneJoe::PosixEventHandle::get_handle()const
{
    return m_handle;
}
#endif
#include "danejoe/network/handle/posix_epoll_handle.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/network/status/posix_status_code.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <sys/epoll.h>

DaneJoe::PosixEpollHandle::PosixEpollHandle() :m_handle(-1) {}

DaneJoe::PosixEpollHandle::PosixEpollHandle(int flags)
{
    m_handle = ::epoll_create1(flags);
}
DaneJoe::PosixEpollHandle::operator bool()
{
    if (m_handle)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void DaneJoe::PosixEpollHandle::init(int flags)
{
    m_handle = ::epoll_create1(flags);
}
DaneJoe::Result<int> DaneJoe::PosixEpollHandle::wait(epoll_event* events, int max_events, int time_out)
{
    if (!m_handle)
    {
        auto status_code = make_posix_status_code(StatusLevel::Error, "Invalid epoll fd!");
        return Result<int>(status_code);
    }
    while (true)
    {
        int ret = ::epoll_wait(m_handle.get(), events, max_events, time_out);
        if (ret < 0)
        {
            auto status_code = make_posix_status_code();
            if (status_code == make_posix_status_code(EINTR))
            {
                continue;
            }
            else
            {
                return Result<int>(status_code);
            }
        }
        else if (ret == 0)
        {
            auto status_code = make_posix_status_code(StatusLevel::Branch);
            return Result<int>(0, status_code);
        }
        else
        {
            auto status_code = make_posix_status_code(StatusLevel::Ok);
            return Result<int>(ret, status_code);
        }
    }
}
DaneJoe::StatusCode DaneJoe::PosixEpollHandle::add(int watched_fd, epoll_event* event)
{
    if (!m_handle)
    {
        return make_posix_status_code(StatusLevel::Error, "Invalid epoll fd!");
    }
    return control(watched_fd, EPOLL_CTL_ADD, event);
}
DaneJoe::StatusCode DaneJoe::PosixEpollHandle::modify(int watched_fd, epoll_event* event)
{
    if (!m_handle)
    {
        return make_posix_status_code(StatusLevel::Error, "Invalid epoll fd!");
    }
    return control(watched_fd, EPOLL_CTL_MOD, event);
}
DaneJoe::StatusCode DaneJoe::PosixEpollHandle::remove(int watched_fd)
{
    if (!m_handle)
    {
        return make_posix_status_code(StatusLevel::Error, "Invalid epoll fd!");
    }
    return control(watched_fd, EPOLL_CTL_DEL, nullptr);
}

DaneJoe::StatusCode DaneJoe::PosixEpollHandle::control(int watched_fd, int operation, epoll_event* event)
{
    if (!m_handle)
    {
        return make_posix_status_code(StatusLevel::Error, "Invalid epoll fd!");
    }
    int ret = ::epoll_ctl(m_handle.get(), operation, watched_fd, event);
    if (ret < 0)
    {
        return make_posix_status_code();
    }
    return make_posix_status_code(StatusLevel::Ok);
}

const DaneJoe::UniqueHandle<int>& DaneJoe::PosixEpollHandle::get_handle()const
{
    return m_handle;
}

#endif

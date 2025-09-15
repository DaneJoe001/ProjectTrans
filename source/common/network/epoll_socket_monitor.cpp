extern "C"
{
#include <sys/epoll.h>
}

#include <cstring>

#include "common/network/epoll_socket_monitor.hpp"
#include "common/network/posix_socket.hpp"
#include "log/manage_logger.hpp"

ISocketMonitor* EpollSocketMonitor::get_instance()
{
    static EpollSocketMonitor instance;
    return &instance;
}

EpollSocketMonitor::EpollSocketMonitor()
{
    m_epoll_fd = epoll_create(1);
    if (m_epoll_fd < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollSocketMonitor", "epoll_create failed");
        return;
    }
}
EpollSocketMonitor::~EpollSocketMonitor()
{
    if (m_epoll_fd >= 0)
    {
        ::close(m_epoll_fd);
    }
}

bool EpollSocketMonitor::check_socket(const ISocket* socket, uint32_t event_type)
{
    if (!socket)
    {
        return false;
    }
    auto fd = *reinterpret_cast<const int*>(dynamic_cast<const PosixSocket*>(socket)->get_raw_socket());
    struct epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.events = EPOLLIN | EPOLLOUT;
    std::lock_guard<std::mutex> lock(m_mutex);
    int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0)
    {
        return false;
    }
    ret = ::epoll_wait(m_epoll_fd, &event, 1, 0);
    uint32_t events = event.events;
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, &event);
    if (ret <= 0)
    {
        return false;
    }
    if (events & event_type)
    {
        return true;
    }
    return false;
}

bool EpollSocketMonitor::is_readable(const ISocket* socket)
{
    return check_socket(socket, EPOLLIN);
}
bool EpollSocketMonitor::is_writeable(const ISocket* socket)
{
    return check_socket(socket, EPOLLOUT);
}
#include "common/network/poll_socket_monitor.hpp"
#include "common/network/posix_socket.hpp"

extern "C"
{
#include <sys/poll.h>
}

bool PollSocketMonitor::check_socket(const ISocket* socket, short event)
{
    if (!socket)
    {
        return false;
    }
    auto fd = *reinterpret_cast<const int*>(dynamic_cast<const PosixSocket*>(socket)->get_raw_socket());
    pollfd pfd{ fd, POLLIN | POLLERR | POLLHUP | POLLRDHUP, 0 };
    int ret = ::poll(&pfd, 1, 0);
    if (ret <= 0) return false;
    if (pfd.revents & (POLLERR | POLLHUP | POLLRDHUP)) return false;
    return (pfd.revents & event) != 0;
}

bool PollSocketMonitor::is_readable(const ISocket* socket)
{
    return check_socket(socket, POLLIN);
}

bool PollSocketMonitor::is_writeable(const ISocket* socket)
{
    return check_socket(socket, POLLOUT);
}
ISocketMonitor* PollSocketMonitor::get_instance()
{
    static PollSocketMonitor instance;
    return &instance;
}
PollSocketMonitor::PollSocketMonitor()
{

}
PollSocketMonitor::~PollSocketMonitor()
{

}
#include "common/network/poll_socket_monitor.hpp"
#include "common/network/posix_socket.hpp"
#include "log/manage_logger.hpp"

extern "C"
{
#include <sys/poll.h>
}

bool PollSocketMonitor::check_socket(const ISocket* socket, int16_t  event)
{
    if (!socket || !socket->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "PollSocketMonitor", "socket is invalid");
        return false;
    }
    // 获取原始套接字
    auto fd = *reinterpret_cast<const int*>(dynamic_cast<const PosixSocket*>(socket)->get_raw_socket());
    // 创建pollfd用于检查套接字
    pollfd pfd{ fd, POLLIN | POLLERR | POLLHUP | POLLRDHUP, 0 };
    // 获取对应的描述符
    int32_t ret = ::poll(&pfd, 1, 0);
    // 检查是否获取成功
    if (ret <= 0) return false;
    // 检查对端是否断开
    if (pfd.revents & (POLLERR | POLLHUP | POLLRDHUP)) return false;
    // 检查是否为对应事件
    return (pfd.revents & event) != 0;
}

bool PollSocketMonitor::is_readable(const ISocket* socket)
{
    // 检查对应套接字的描述符是否可读
    return check_socket(socket, POLLIN);
}

bool PollSocketMonitor::is_writable(const ISocket* socket)
{
    // 检查对应套接字的描述符是否可写
    return check_socket(socket, POLLOUT);
}
ISocketMonitor* PollSocketMonitor::get_instance()
{
    // 创建实例
    static PollSocketMonitor instance;
    return &instance;
}
PollSocketMonitor::PollSocketMonitor()
{

}
PollSocketMonitor::~PollSocketMonitor()
{

}
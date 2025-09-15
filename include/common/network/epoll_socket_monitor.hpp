#pragma once

#include <mutex>
#include <cstdint>

#include  "common/network/i_socket_monitor.hpp"

class EpollSocketMonitor : public ISocketMonitor
{
public:
    static ISocketMonitor* get_instance();
    bool is_readable(const ISocket* socket) override;
    bool is_writeable(const ISocket* socket) override;
private:
    bool check_socket(const ISocket* socket, uint32_t event_type);
private:
    EpollSocketMonitor();
    ~EpollSocketMonitor();
private:
    std::mutex m_mutex;
    int32_t m_epoll_fd = -1;

};
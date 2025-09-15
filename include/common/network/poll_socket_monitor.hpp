#pragma once

#include <cstdint>

#include "common/network/i_socket_monitor.hpp"

class PollSocketMonitor : public ISocketMonitor {
public:
    bool is_readable(const ISocket* socket) override;
    bool is_writeable(const ISocket* socket) override;
    static ISocketMonitor* get_instance();
private:
    bool check_socket(const ISocket* socket, int16_t  event);
    PollSocketMonitor();
    ~PollSocketMonitor();
private:

};
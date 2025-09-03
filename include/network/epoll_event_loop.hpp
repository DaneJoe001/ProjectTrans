#pragma once

#include "network/i_event_loop.hpp"

class EpollEventLoop : public IEventLoop
{
public:
    void add_socket(std::unique_ptr<IClientSocket> socket) override;
    void remove_socket(std::unique_ptr<IClientSocket> socket) override;
    void run() override;
private:
    int m_epoll_fd = -1;
};
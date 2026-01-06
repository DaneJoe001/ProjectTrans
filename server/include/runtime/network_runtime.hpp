#pragma once

#include <memory>

#include "danejoe/network/event_loop/posix_epoll_event_loop.hpp"
#include "danejoe/network/runtime/reactor_mail_box.hpp"

class NetworkRuntime
{
public:
    NetworkRuntime(
        std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box);
    void init();
    bool is_init() const;
    void run();
    void stop();
private:
    DaneJoe::PosixEpollEventLoop m_event_loop;
    std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;
    bool m_is_init = false;
};


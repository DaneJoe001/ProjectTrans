#pragma once

#include <memory>
#include <atomic>
#include <unordered_map>

#include "danejoe/network/handle/posix_epoll_handle.hpp"
#include "danejoe/network/handle/posix_event_handle.hpp"
#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/context/connect_context.hpp"
#include "danejoe/network/runtime/reactor_mail_box.hpp"

namespace DaneJoe
{
    class PosixEpollEventLoop
    {
    public:
        PosixEpollEventLoop();
        PosixEpollEventLoop(
            std::shared_ptr<ReactorMailBox> reactor_mail_box,
            std::shared_ptr<PosixEventHandle> event_handle,
            PosixSocketHandle&& server_handle,
            PosixEpollHandle&& epoll_handle);
        ~PosixEpollEventLoop();
        void init(
            std::shared_ptr<ReactorMailBox> reactor_mail_box,
            std::shared_ptr<PosixEventHandle> event_handle,
            PosixSocketHandle&& server_handle,
            PosixEpollHandle&& epoll_handle);
        void run();
        void stop();
        void notify();
        void remove_connect(int fd);
        void readable_event(int fd);
        void writable_event(int fd);
        void acceptable_event();
        void notify_event();
    private:
        int m_max_event_counts = 1024;
        std::atomic<bool> m_is_running = false;
        std::atomic<uint64_t> m_connect_counter = 0;
        std::shared_ptr<ReactorMailBox> m_reactor_mail_box = nullptr;
        std::unordered_map<int, ConnectContext> m_connect_contexts;
        PosixEpollHandle m_epoll_handle;
        std::shared_ptr<PosixEventHandle> m_event_handle;
        PosixSocketHandle m_server_handle;
    };
}
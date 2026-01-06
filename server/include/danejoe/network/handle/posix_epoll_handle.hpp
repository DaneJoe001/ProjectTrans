#pragma once

#include "danejoe/common/result/result.hpp"
#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

#if DANEJOE_PLATFORM_LINUX==1

struct epoll_event;

namespace DaneJoe
{
    class PosixEpollHandle
    {
    public:
        PosixEpollHandle();
        PosixEpollHandle(int flags);
        operator bool();
        void init(int flags);
        Result<int> wait(epoll_event* events, int max_events, int time_out);
        StatusCode add(int watched_fd, epoll_event* event);
        StatusCode modify(int watched_fd, epoll_event* event);
        StatusCode remove(int watched_fd);
        StatusCode control(int watched_fd, int operation, epoll_event* event);
        const UniqueHandle<int>& get_handle()const;
    private:
        UniqueHandle<int> m_handle;
    };
};

#endif
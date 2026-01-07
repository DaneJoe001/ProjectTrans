#include "danejoe/common/type_traits/platform_traits.hpp"

#if DANEJOE_PLATFORM_LINUX==1

#include <vector>
#include <cerrno>
#include <cstring>

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/status/posix_status_code.hpp"
#include "danejoe/network/event_loop/posix_epoll_event_loop.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

DaneJoe::PosixEpollEventLoop::PosixEpollEventLoop() {}

DaneJoe::PosixEpollEventLoop::PosixEpollEventLoop(
    std::shared_ptr<ReactorMailBox> reactor_mail_box,
    std::shared_ptr<PosixEventHandle> event_handle,
    PosixSocketHandle&& server_handle,
    PosixEpollHandle&& epoll_handle) :
    m_reactor_mail_box(reactor_mail_box),
    m_epoll_handle(std::move(epoll_handle)),
    m_event_handle(event_handle),
    m_server_handle(std::move(server_handle))
{
    epoll_event server_event;
    server_event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    server_event.data.fd=m_server_handle.get_handle().get();
    m_epoll_handle.add(m_server_handle.get_handle().get(), &server_event);
    epoll_event event_event;
    event_event.events = EPOLLIN;
    event_event.data.fd=m_event_handle->get_handle().get();
    m_epoll_handle.add(m_event_handle->get_handle().get(), &event_event);
}
DaneJoe::PosixEpollEventLoop::~PosixEpollEventLoop()
{
    stop();
}
void DaneJoe::PosixEpollEventLoop::init(
    std::shared_ptr<ReactorMailBox> reactor_mail_box,
    std::shared_ptr<PosixEventHandle> event_handle,
    PosixSocketHandle&& server_handle,
    PosixEpollHandle&& epoll_handle)
{
    m_reactor_mail_box = reactor_mail_box;
    m_event_handle = event_handle;
    m_epoll_handle = std::move(epoll_handle);
    m_server_handle = std::move(server_handle);
    epoll_event server_event;
    server_event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    server_event.data.fd = m_server_handle.get_handle().get();
    {
        auto st = m_epoll_handle.add(m_server_handle.get_handle().get(), &server_event);
        if (st.get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_ERROR("network", "epoll add server fd failed: fd={}, status={}",
                m_server_handle.get_handle().get(),
                st.message());
        }
    }
    epoll_event event_event;
    event_event.events = EPOLLIN;
    event_event.data.fd = m_event_handle->get_handle().get();
    {
        auto st = m_epoll_handle.add(m_event_handle->get_handle().get(), &event_event);
        if (st.get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_ERROR("network", "epoll add eventfd failed: fd={}, status={}",
                m_event_handle->get_handle().get(),
                st.message());
        }
    }
}
void DaneJoe::PosixEpollEventLoop::run()
{
    if (!m_reactor_mail_box || !m_epoll_handle || !m_event_handle || !m_server_handle)
    {
        ADD_DIAG_ERROR("network", "PosixEpollEventLoop run skipped: invalid handles (mailbox={}, epoll={}, eventfd={}, server={})",
            static_cast<void*>(m_reactor_mail_box.get()),
            m_epoll_handle.get_handle().get(),
            m_event_handle ? m_event_handle->get_handle().get() : -1,
            m_server_handle.get_handle().get());
        return;
    }
    ADD_DIAG_INFO("network", "PosixEpollEventLoop started: epoll_fd={}, server_fd={}, event_fd={}",
        m_epoll_handle.get_handle().get(),
        m_server_handle.get_handle().get(),
        m_event_handle->get_handle().get());
    m_is_running.store(true);
    auto events =
        std::vector<epoll_event>(m_max_event_counts);
    while (m_is_running)
    {
        auto ret = m_epoll_handle.wait(events.data(), m_max_event_counts, 1000);
        if (ret.status_code().get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_ERROR("network", "Run loop failed: epoll_wait failed");
            return;
        }
        else if (ret.status_code().get_status_level() == StatusLevel::Branch)
        {
            continue;
        }
        if (!ret.has_value())
        {
            ADD_DIAG_ERROR("network", "Run loop failed: epoll_wait failed");
            return;
        }
        for (int i = 0;i < ret.value(); i++)
        {
            int fd = events[i].data.fd;
            uint32_t event = events[i].events;
            if (fd == m_server_handle.get_handle().get())
            {
                ADD_DIAG_DEBUG("network", "epoll event: server fd readable");
                acceptable_event();
                continue;
            }
            if (fd == m_event_handle->get_handle().get())
            {
                ADD_DIAG_DEBUG("network", "epoll event: eventfd readable");
                notify_event();
                continue;
            }

            ADD_DIAG_DEBUG("network", "epoll event: client fd={}, events=0x{:x}", fd, event);

            if (event & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
            {
                ADD_DIAG_WARN("network", "Handle client event: client socket error or closed");
                remove_connect(fd);
                continue;
            }
            // 客户端读事件
            if (event & EPOLLIN)
            {
                ADD_DIAG_DEBUG("network", "epoll event: client fd readable, fd={}", fd);
                readable_event(fd);
            }
            // 客户端写事件：仅在需要 flush 待发送缓冲时才会开启 EPOLLOUT，避免 busy-loop。
            if (event & EPOLLOUT)
            {
                ADD_DIAG_DEBUG("network", "epoll event: client fd writable, fd={}", fd);
                writable_event(fd);
            }
        }
    }
    ADD_DIAG_WARN("network", "PosixEpollEventLoop exited");
}
void DaneJoe::PosixEpollEventLoop::stop()
{
    m_is_running.store(false);
}
void DaneJoe::PosixEpollEventLoop::remove_connect(int fd)
{
    {
        auto st = m_epoll_handle.remove(fd);
        if (st.get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_WARN("network", "epoll remove client fd failed: fd={}, status={}", fd, st.message());
        }
    }
    auto context_it = m_connect_contexts.find(fd);
    if (context_it == m_connect_contexts.end())
    {
        return;
    }
    m_reactor_mail_box->remove_to_client_queue(context_it->second.get_connect_id());
    m_connect_contexts.erase(context_it);
}
void DaneJoe::PosixEpollEventLoop::notify()
{
    if (!m_event_handle)
    {
        return;
    }
    uint64_t value = 1;
    auto ret = m_event_handle->write(value);
    if (ret.status_code().get_status_level() == StatusLevel::Error)
    {
        ADD_DIAG_WARN("PosixEpollEventLoop", "Faild to notify!");
        return;
    }
}
void DaneJoe::PosixEpollEventLoop::readable_event(int fd)
{
    if (!m_reactor_mail_box)
    {
        return;
    }
    auto context_it = m_connect_contexts.find(fd);
    if (context_it == m_connect_contexts.end())
    {
        return;
    }
    auto ret = context_it->second.read();
    if (ret.status_code().get_status_level() == StatusLevel::Error)
    {
        ADD_DIAG_WARN("network", "readable_event: read error, fd={}, remove connect", fd);
        remove_connect(fd);
        return;
    }
    if (!ret.has_value())
    {
        return;
    }
    if (!ret.value().empty())
    {
        ADD_DIAG_DEBUG("network", "readable_event: received frames fd={}, count={}", fd, static_cast<int>(ret.value().size()));
    }
    m_reactor_mail_box->push_to_server_frame(ret.value());
}
void DaneJoe::PosixEpollEventLoop::writable_event(int fd)
{
    if (!m_reactor_mail_box)
    {
        return;
    }
    auto context_it = m_connect_contexts.find(fd);
    if (context_it == m_connect_contexts.end())
    {
        return;
    }
    std::vector<PosixFrame> frames;
    while (true)
    {
        auto frame_opt =
            m_reactor_mail_box->pop_from_to_client_queue(context_it->second.get_connect_id());
        if (!frame_opt.has_value())
        {
            break;
        }
        frames.push_back(frame_opt.value());
    }
    auto ret = context_it->second.write(frames);
    if (ret.status_code().get_status_level() == StatusLevel::Error)
    {
        ADD_DIAG_WARN("network", "writable_event: write error, fd={}, remove connect", fd);
        remove_connect(fd);
        return;
    }

    // 若 non-blocking 写未写完（m_write_buffer 仍有数据），需要开启 EPOLLOUT 等待可写继续 flush。
    // 反之则关闭 EPOLLOUT，避免 socket 一直处于“可写”导致空转。
    {
        epoll_event event;
        event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
        if (context_it->second.has_pending_write())
        {
            event.events |= EPOLLOUT;
        }
        event.data.fd = fd;
        auto st = m_epoll_handle.modify(fd, &event);
        if (st.get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_WARN("network", "writable_event: epoll modify client fd failed: fd={}, status={}", fd, st.message());
        }
    }
}
void DaneJoe::PosixEpollEventLoop::acceptable_event()
{
    if (!m_reactor_mail_box || !m_server_handle)
    {
        return;
    }
    while (true)
    {
        auto ret =
            m_server_handle.accept();
        if (!ret.has_value())
        {
            if (ret.status_code().get_status_level() == StatusLevel::Error)
            {
                ADD_DIAG_WARN("network", "accept failed: {}", ret.status_code().message());
            }
            return;
        }
        auto status_code =
            ret.value().set_blocking(false);
        if (status_code.get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_WARN("PosixEpollEventLoop", "Failed to set non blocking！");
            return;
        }
        int fd = ret.value().get_handle().get();
        epoll_event event;
        event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
        event.data.fd = fd;
        {
            auto st = m_epoll_handle.add(fd, &event);
            if (st.get_status_level() == StatusLevel::Error)
            {
                ADD_DIAG_ERROR("network", "epoll add client fd failed: fd={}, status={}", fd, st.message());
                return;
            }
        }

        {
            sockaddr_in peer_addr;
            socklen_t peer_len = sizeof(peer_addr);
            std::memset(&peer_addr, 0, sizeof(peer_addr));
            if (::getpeername(fd, reinterpret_cast<sockaddr*>(&peer_addr), &peer_len) == 0)
            {
                char ip_buf[INET_ADDRSTRLEN];
                const char* ip = ::inet_ntop(AF_INET, &peer_addr.sin_addr, ip_buf, sizeof(ip_buf));
                const int port = static_cast<int>(::ntohs(peer_addr.sin_port));
                ADD_DIAG_DEBUG("network", "acceptable_event: peer addr: fd={}, ip={}, port={}",
                    fd,
                    (ip ? ip : "<invalid>"),
                    port);
            }
            else
            {
                ADD_DIAG_DEBUG("network", "acceptable_event: getpeername failed: fd={}, errno={}, err={}",
                    fd,
                    errno,
                    std::strerror(errno));
            }

            uint8_t peek_buf[64];
            ssize_t peek_ret = ::recv(fd, peek_buf, sizeof(peek_buf), MSG_PEEK | MSG_DONTWAIT);
            if (peek_ret > 0)
            {
                ADD_DIAG_DEBUG("network", "acceptable_event: peek bytes available on new connection: fd={}, peek_size={}",
                    fd,
                    static_cast<int>(peek_ret));
            }
            else if (peek_ret == 0)
            {
                ADD_DIAG_DEBUG("network", "acceptable_event: peer closed immediately after accept: fd={}", fd);
            }
            else
            {
                ADD_DIAG_DEBUG("network", "acceptable_event: peek no data yet: fd={}, errno={}, err={}",
                    fd,
                    errno,
                    std::strerror(errno));
            }
        }

        auto connect_id = m_connect_counter++;
        m_connect_contexts.emplace(fd, ConnectContext{ connect_id, std::move(ret.value()) });
        m_reactor_mail_box->add_to_client_queue(connect_id);
        ADD_DIAG_INFO("network", "accept new connection: fd={}, connect_id={}", fd, connect_id);
    }
}
void DaneJoe::PosixEpollEventLoop::notify_event()
{
    if (!m_event_handle)
    {
        return;
    }
    int drained = 0;
    while (true)
    {
        auto ret = m_event_handle->read();
        if (ret.status_code().get_status_level() == StatusLevel::Error)
        {
            return;
        }
        if (ret.status_code() == make_posix_status_code(EAGAIN))
        {
            break;
        }
        drained++;
    }
    if (drained > 0)
    {
        ADD_DIAG_TRACE("network", "notify_event: drained eventfd count={}", drained);
    }
    for (const auto& it : m_connect_contexts)
    {
        writable_event(it.first);
    }
}

#endif
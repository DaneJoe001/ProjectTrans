#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/network/handle/posix_epoll_handle.hpp"
#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/handle/posix_event_handle.hpp"
#include "runtime/network_runtime.hpp"

extern "C"
{
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
}

NetworkRuntime::NetworkRuntime(
    std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box) :
    m_reactor_mail_box(reactor_mail_box)
{}

bool NetworkRuntime::is_init() const
{
    return m_is_init;
}

void NetworkRuntime::init()
{
    m_is_init = false;
    DANEJOE_LOG_INFO("default", "NetworkRuntime", "Init network runtime");
    auto event_handle = std::make_shared<DaneJoe::PosixEventHandle>();
    event_handle->init(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (!(*event_handle))
    {
        DANEJOE_LOG_ERROR("default", "NetworkRuntime", "Failed to create eventfd");
        return;
    }
    DANEJOE_LOG_DEBUG("default", "NetworkRuntime", "eventfd created, fd={}", event_handle->get_handle().get());
    m_reactor_mail_box->set_event_handle(event_handle);
    DaneJoe::PosixEpollHandle epoll_handle;
    epoll_handle.init(EPOLL_CLOEXEC);
    if (!epoll_handle)
    {
        DANEJOE_LOG_ERROR("default", "NetworkRuntime", "Failed to create epoll fd");
        return;
    }
    DANEJOE_LOG_DEBUG("default", "NetworkRuntime", "epoll fd created, fd={}", epoll_handle.get_handle().get());
    DaneJoe::PosixSocketHandle server_handle(AF_INET, SOCK_STREAM, 0);
    auto set_non_blocking_status =
        server_handle.set_blocking(false);
    if (set_non_blocking_status.get_status_level() == DaneJoe::StatusLevel::Error)
    {
        DANEJOE_LOG_ERROR("default", "NetworkRuntime", "Failed to set server socket non blocking");
        return;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = ::htons(8080);
    address.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    auto bind_status =
        server_handle.bind((struct sockaddr*)&address, sizeof(address));
    if (bind_status.get_status_level() == DaneJoe::StatusLevel::Error)
    {
        DANEJOE_LOG_ERROR("default", "NetworkRuntime", "Failed to bind ip port");
        return;
    }
    DANEJOE_LOG_INFO("default", "NetworkRuntime", "Bind success: 127.0.0.1:8080");
    auto listen_status =
        server_handle.listen(5);
    if (listen_status.get_status_level() == DaneJoe::StatusLevel::Error)
    {
        DANEJOE_LOG_ERROR("default", "NetworkRuntime", "Failed to listen");
        return;
    }
    DANEJOE_LOG_INFO("default", "NetworkRuntime", "Listen success, backlog=5");
    m_event_loop.init(m_reactor_mail_box, event_handle, std::move(server_handle), std::move(epoll_handle));
    DANEJOE_LOG_INFO("default", "NetworkRuntime", "Event loop initialized");
    m_is_init = true;
}
void NetworkRuntime::run()
{
    DANEJOE_LOG_INFO("default", "NetworkRuntime", "Network runtime thread started");
    m_event_loop.run();
    DANEJOE_LOG_WARN("default", "NetworkRuntime", "Network runtime thread exited");
}

void NetworkRuntime::stop()
{
    m_event_loop.stop();
}
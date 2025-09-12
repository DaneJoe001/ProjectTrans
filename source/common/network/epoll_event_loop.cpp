#include <cstring>
#include <cerrno>
#include <memory>

extern "C"
{
#include <sys/epoll.h>
#include <unistd.h>
}

#include "common/network/epoll_event_loop.hpp"
#include "common/network/posix_client_socket.hpp"
#include "log/manage_logger.hpp"

/// @todo 将读写引入线程中，避免顺序执行导致缓冲区满阻塞

EpollEventLoop::EpollEventLoop(std::unique_ptr<PosixServerSocket> server_socket, std::unique_ptr<ISocketContextCreator> context_creator)
{
    /// @brief 服务器套接字
    m_server_socket = std::move(server_socket);
    /// @brief 确保服务器套接字为非阻塞模式
    bool is_set_non_blocking = m_server_socket->set_non_blocking(true);
    if (!is_set_non_blocking)
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "Failed to set server socket non-blocking");
        stop();
        return;
    }
    /// @brief 业务上下文创建器
    m_context_creator = std::move(context_creator);
    /// @brief 创建epoll 文件描述符
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd < 0)
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "epoll_create1 failed");
        stop();
        return;
    }
    /// @brief 在server socket有效时初始化其他部分
    if (m_server_socket->is_valid())
    {
        struct epoll_event event;
        std::memset(&event, 0, sizeof(event));
        /// @brief  事件类型：文件描述符可读/非阻塞的事件触发策略（Linux 内核 2.6.22 引入）
        event.events = EPOLLIN;
        event.data.fd = *reinterpret_cast<const int*>(m_server_socket->get_raw_socket());
        /// @brief 添加文件描述符
        int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, *reinterpret_cast<const int*>(m_server_socket->get_raw_socket()), &event);
        if (ret < 0)
        {
            DANEJOE_LOG_ERROR("default", "Epoll", "epoll_ctl failed");
            stop();
            return;
        }
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "server socket is not valid");
        stop();
        return;
    }
}

EpollEventLoop::~EpollEventLoop()
{
    stop();
}

bool EpollEventLoop::add_socket(std::unique_ptr<IClientSocket> socket, EventType type)
{
    if (!socket)
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "socket is null");
        return false;
    }
    /// @brief 设置套接字需要关注的事件
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    /// @brief 转换Epoll事件类型至抽象事件类型
    ev.events = to_epoll_events(type);
    /// @brief 获取客户端套接字文件描述符
    int socket_fd = *reinterpret_cast<const int*>(socket->get_raw_socket());
    ev.data.fd = socket_fd;
    int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "add socket to epoll failed");
        socket->close();
        return false;
    }
    /// @brief 初始化对应的接收缓存
    int socket_id = socket->get_id();
    m_recv_buffers[socket_id] = std::make_shared<DaneJoe::MTQueue<uint8_t>>(4096);
    m_send_buffers[socket_id] = std::make_shared<DaneJoe::MTQueue<uint8_t>>(4096);
    m_sockets[socket_id] = std::move(socket);
    m_socket_events[socket_id] = type;
    m_contexts[socket_id] = m_context_creator->create();
    return true;
}
void EpollEventLoop::remove_socket(int socket_id)
{
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "epoll is not valid");
        return;
    }
    auto socket_it = m_sockets.find(socket_id);
    if (socket_it == m_sockets.end())
    {
        DANEJOE_LOG_WARN("default", "Epoll", "Socket {} not found", socket_id);
        return;
    }
    auto client = std::move(socket_it->second);
    auto raw_socket = *reinterpret_cast<const int*>(client->get_raw_socket());
    int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, raw_socket, nullptr);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "epoll_ctl error");
    }
    client->close();
    m_recv_buffers.erase(socket_id);
    m_send_buffers.erase(socket_id);
    m_sockets.erase(socket_id);
    m_contexts.erase(socket_id);
    m_socket_events.erase(socket_id);
}
void EpollEventLoop::run()
{
    /// @brief 在epoll标识符无效时直接退出
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "Epoll", "epoll is not valid");
        return;
    }
    m_is_running = true;
    struct epoll_event events[m_max_event_account];
    while (m_is_running)
    {
        /// @brief 等待事件发生
        /// @brief epoll 文件描述符
        /// @brief 监听事件
         /// @brief 最大监听事件数量
        /// @brief 等待时间,-1 表示一直等待
        int ret = ::epoll_wait(m_epoll_fd, events, m_max_event_account, 1000);
        if (ret < 0)
        {
            /// @brief 忽略 EINTR 错误
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                DANEJOE_LOG_ERROR("default", "Epoll", "epoll_wait failed");
                break;
            }
        }
        for (int i = 0;i < ret;i++)
        {
            /// @brief 获取文件描述符
            int fd = events[i].data.fd;
            /// @brief 获取对应文件描述符触发的事件
            uint32_t event = events[i].events;
            /// @brief 判断是否为服务器 socket
            if (*reinterpret_cast<const int*>(m_server_socket->get_raw_socket()) == fd)
            {
                acceptable_event();
            }
            /// @brief 错误或对端关闭：及时回收 fd，避免无意义的后续处理
            if (event & (EPOLLHUP | EPOLLERR))
            {
                DANEJOE_LOG_WARN("default", "Epoll", "client socket error or closed!");
                remove_socket(PosixClientSocket::get_id(fd));
                continue;
            }
            /// @brief 客户端读事件
            if (event & EPOLLIN)
            {
                readable_event(fd);
            }
            /// @brief 客户端写事件
            if (event & EPOLLOUT)
            {
                writable_event(fd);
            }
        }
    }
}

void EpollEventLoop::stop()
{
    m_is_running = false;
    // 关闭所有客户端套接字
    for (auto& [socket_id, socket] : m_sockets)
    {
        if (socket)
        {
            socket->close();
        }
    }
    // 清理所有映射
    m_sockets.clear();
    m_recv_buffers.clear();
    m_send_buffers.clear();
    m_contexts.clear();
    m_socket_events.clear();

    if (is_valid())
    {
        ::close(m_epoll_fd);
        m_epoll_fd = -1;
    }
}

bool EpollEventLoop::is_valid()
{
    return m_epoll_fd >= 0;
}

uint32_t EpollEventLoop::to_epoll_events(EventType type)
{
    uint32_t event = 0;

    if (is_event_valid(type & EventType::Readable))
    {
        event |= EPOLLIN;
    }
    if (is_event_valid(type & EventType::Writable))
    {
        event |= EPOLLOUT;
    }
    if (is_event_valid(type & EventType::Priority))
    {
        event |= EPOLLPRI;
    }
    if (is_event_valid(type & EventType::Disconnected))
    {
        event |= EPOLLHUP;
    }
    if (is_event_valid(type & EventType::EdgeTriggered))
    {
        event |= EPOLLET;
    }
    if (is_event_valid(type & EventType::OneShot))
    {
        event |= EPOLLONESHOT;
    }
    if (is_event_valid(type & EventType::Error))
    {
        event |= EPOLLERR;
    }
    if (is_event_valid(type & EventType::PeerClosed))
    {
        event |= EPOLLRDHUP;
    }
    return event;
}

void EpollEventLoop::acceptable_event()
{
    /// @brief 边缘触发模式要求在套接字就绪时必须尽可能多地处理所有可用的连接
    while (true)
    {
        /// @brief 尝试接受客户端连接
        auto client = m_server_socket->accept();
        if (!client)
        {
            /// @brief 如果没有更多连接，则退出循环
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            DANEJOE_LOG_ERROR("default", "Epoll", "accept failed: {}", strerror(errno));
            break;
        }
        /// @brief 确保设置客户端套接字非阻塞
        if (!client->set_non_blocking(true))
        {
            DANEJOE_LOG_ERROR("default", "Epoll", "set non blocking failed");
            client->close();
            continue;
        }
        /// @note 更符合服务器的工作逻辑，确保在处理新连接时仅关注可读事件，而不是可写事件。
        if (!add_socket(std::move(client), EventType::Readable | EventType::EdgeTriggered | EventType::PeerClosed))
        {
            DANEJOE_LOG_ERROR("default", "Epoll", "add socket failed");
            continue;
        }
        DANEJOE_LOG_TRACE("default", "Epoll", "add socket success");
        continue;
    }
}

void EpollEventLoop::readable_event(int fd)
{
    /// @brief 当前posix实现下，socket_map的键就是(int)fd
    auto socket_iter = m_sockets.find(fd);
    if (socket_iter == m_sockets.end() || socket_iter->second == nullptr)
    {
        remove_socket(PosixClientSocket::get_id(fd));
        return;
    }
    std::vector<uint8_t> data = m_sockets.at(fd)->read_all();
    auto buffer = m_recv_buffers.at(PosixClientSocket::get_id(fd));
    buffer->push(data.begin(), data.end());
    m_contexts.at(PosixClientSocket::get_id(fd))->on_recv(buffer);

    if (!buffer->full())
    {
        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        /// @brief 监听可写事件
        ev.events = EPOLLOUT;
        ev.data.fd = fd;
        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        {
            DANEJOE_LOG_ERROR("default", "Epoll", "epoll_ctl failed!");
        }
    }
}

void EpollEventLoop::writable_event(int fd)
{
    /// @brief 当前posix实现下，socket_map的键就是(int)fd
    auto client_iter = m_sockets.find(fd);
    if (client_iter == m_sockets.end() || client_iter->second == nullptr)
    {
        remove_socket(PosixClientSocket::get_id(fd));
        return;
    }
    /// @brief 获取发送缓存
    auto buffer = m_send_buffers.at(PosixClientSocket::get_id(fd));

    /// @brief 调用发送回调
    m_contexts.at(PosixClientSocket::get_id(fd))->on_send(buffer);
    /// @brief 获取发送数据
    std::vector<uint8_t> data = buffer->try_pop(1024);
    /// @brief 发送数据
    m_sockets.at(fd)->send_all(data);
    /// @brief 写完后修改监听事件
    if (buffer->empty())
    {
        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        /// @brief 监听可读事件
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        {
            DANEJOE_LOG_ERROR("default", "Epoll", "epoll_ctl failed!");
        }
    }
}
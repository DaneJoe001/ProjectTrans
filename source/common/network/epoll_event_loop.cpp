#include <cstring>
#include <cerrno>
#include <memory>
#include <cerrno>

extern "C"
{
#include <sys/epoll.h>
#include <unistd.h>
}

#include "common/network/epoll_event_loop.hpp"
#include "common/network/posix_client_socket.hpp"
#include "common/log/manage_logger.hpp"

/// @todo 将读写引入线程中，避免顺序执行导致缓冲区满阻塞

#define USE_SERVER_TRANS
#define MODIFY_SOCKET_EVENT
EpollEventLoop::EpollEventLoop(std::unique_ptr<PosixServerSocket> server_socket, std::unique_ptr<ISocketContextCreator> context_creator)
{
    // 先检查服务器套接字是否不为空且有效
    if (!server_socket || !(server_socket->is_valid()))
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to create epoll: server socket is null");
        return;
    }
    // 服务器套接字
    m_server_socket = std::move(server_socket);
    m_server_socket->set_non_blocking(true);
    // 确保服务器套接字为非阻塞模式
    bool is_set_non_blocking = m_server_socket->set_non_blocking(true);
    // 设置失败则退出
    if (!is_set_non_blocking)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to set server socket non-blocking");
        stop();
        return;
    }
    // 业务上下文创建器
    m_context_creator = std::move(context_creator);
    // 创建epoll 文件描述符
    m_epoll_fd = epoll_create1(0);
    // 创建失败则退出
    if (m_epoll_fd < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "epoll_create1 failed");
        stop();
        return;
    }
    // 创建并初始化epoll_event结构
    struct epoll_event event = {};
    // 获取服务器原始套接字
    int32_t raw_server_fd = *reinterpret_cast<const int*>(m_server_socket->get_raw_socket());
    // 事件类型：文件描述符可读
    event.events = EPOLLIN;
    event.data.fd = raw_server_fd;
    // 注册服务器套接字
    int32_t ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, raw_server_fd, &event);
    // 注册失败则停止并退出
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to add server socket to epoll: {}", std::strerror(errno));
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
    // 检查自身是否有效
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Epoll is invalid");
        return false;
    }
    // 检查套接字是否为空
    if (!socket)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "socket is null");
        return false;
    }
    // 检查套接字是否有效
    if (!socket->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "socket is invalid");
        return false;
    }
    // 设置套接字需要关注的事件
    struct epoll_event ev = {};
    // 转换Epoll事件类型至抽象事件类型
    ev.events = to_epoll_events(type);
    // 获取客户端套接字文件描述符
    int32_t socket_fd = *reinterpret_cast<const int*>(socket->get_raw_socket());
    ev.data.fd = socket_fd;
    // 添加客户端套接字监听
    int32_t ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
    // 添加失败则关闭套接字并返回失败
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to add socket to epoll: {}", std::strerror(errno));
        socket->close();
        return false;
    }
    // 初始化对应的接收缓存
    int32_t socket_id = socket->get_id();
    m_recv_buffers[socket_id] = std::make_shared<DaneJoe::MTQueue<uint8_t>>(4096);
    m_send_buffers[socket_id] = std::make_shared<DaneJoe::MTQueue<uint8_t>>(4096);
    m_sockets[socket_id] = std::move(socket);
    m_socket_events[socket_id] = type;
    m_contexts[socket_id] = m_context_creator->create(m_recv_buffers[socket_id], m_send_buffers[socket_id]);
    return true;
}
void EpollEventLoop::remove_socket(int32_t socket_id)
{
    // 检查自身是否有效
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Epoll is invalid");
        return;
    }
    // 获取套接字迭代器
    auto socket_it = m_sockets.find(socket_id);
    // 检查映射表中是否存在记录
    if (socket_it == m_sockets.end())
    {
        DANEJOE_LOG_WARN("default", "EpollEventLoop", "Socket {} not found", socket_id);
        return;
    }
    // 获取套接字
    auto client = std::move(socket_it->second);
    // 获取原始套接字
    auto raw_socket = *reinterpret_cast<const int*>(client->get_raw_socket());
    // 删除epoll对套接字的监听
    int32_t ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, raw_socket, nullptr);
    // 检查移除是否成功
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to remove socket from epoll: {}", std::strerror(errno));
    }
    m_context_creator->destroy(m_contexts[socket_id]);
    // 清理与套接字配对的资源
    client->close();
    m_recv_buffers.erase(socket_id);
    m_send_buffers.erase(socket_id);
    m_sockets.erase(socket_id);
    m_contexts.erase(socket_id);
    m_socket_events.erase(socket_id);
}
void EpollEventLoop::run()
{
    // 检查自身是否有效
    if (!is_valid())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Epoll is invalid");
        return;
    }
    // 更新运行标志
    m_is_running.store(true);
    // 创建事件容器
    struct epoll_event events[m_max_event_count];
    while (m_is_running.load())
    {
        // 等待事件发生
        int32_t ret = ::epoll_wait(m_epoll_fd, events, m_max_event_count, 1000);
        if (ret < 0)
        {
            // 忽略 EINTR 错误
            if (errno == EINTR)
            {
                continue;
            }
            // 其他错误退出事件循环
            else
            {
                DANEJOE_LOG_ERROR("default", "EpollEventLoop", "epoll_wait failed");
                break;
            }
        }
        for (int32_t i = 0;i < ret;i++)
        {
            // 获取文件描述符
            int32_t fd = events[i].data.fd;
            // 获取对应文件描述符触发的事件
            uint32_t event = events[i].events;
            // 获取服务器原始套接字
            int32_t server_fd = *reinterpret_cast<const int*>(m_server_socket->get_raw_socket());
            if (server_fd == fd)
            {
                // 处理服务端接收事件
                acceptable_event();
                continue;
            }
            // 错误或对端关闭：及时回收 fd，避免无意义的后续处理
            // EPOLLHUP hand up 挂起即连接被关闭
            if (event & (EPOLLHUP | EPOLLERR))
            {
                DANEJOE_LOG_WARN("default", "EpollEventLoop", "client socket error or closed!");
                remove_socket(PosixClientSocket::get_id(fd));
                continue;
            }
            // 客户端读事件
            if (event & EPOLLIN)
            {
                readable_event(PosixClientSocket::get_id(fd));
            }
            // 客户端写事件
            if (event & EPOLLOUT)
            {
                writable_event(PosixClientSocket::get_id(fd));
            }
        }
    }
}

void EpollEventLoop::stop()
{
    // 结束事件循环
    end_loop();
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
    // 当Epoll标识符有效时进行关闭
    if (is_valid())
    {
        ::close(m_epoll_fd);
        m_epoll_fd = -1;
    }
}

bool EpollEventLoop::is_valid()
{
    // 返回标识符是否大于等于0
    return m_epoll_fd >= 0;
}

uint32_t EpollEventLoop::to_epoll_events(EventType type)
{
    uint32_t event = 0;

    if (is_event_valid(type & EventType::Readable))
    {
        // 可读事件
        event |= EPOLLIN;
    }
    if (is_event_valid(type & EventType::Writable))
    {
        // 可写事件
        event |= EPOLLOUT;
    }
    if (is_event_valid(type & EventType::Priority))
    {
        // 优先级事件
        event |= EPOLLPRI;
    }
    if (is_event_valid(type & EventType::Disconnected))
    {
        // 断开事件
        event |= EPOLLHUP;
    }
    if (is_event_valid(type & EventType::EdgeTriggered))
    {
        // 边缘触发
        event |= EPOLLET;
    }
    if (is_event_valid(type & EventType::OneShot))
    {
        // 单次触发
        event |= EPOLLONESHOT;
    }
    if (is_event_valid(type & EventType::Error))
    {
        // 错误事件
        event |= EPOLLERR;
    }
    if (is_event_valid(type & EventType::PeerClosed))
    {
        // 对端关闭
        event |= EPOLLRDHUP;
    }
    return event;
}

void EpollEventLoop::acceptable_event()
{
    // 先检查服务器套接字是否不为空且有效
    if (!m_server_socket || !m_server_socket->is_valid())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Server socket is null");
        return;
    }
    // 边缘触发模式要求在套接字就绪时必须尽可能多地处理所有可用的连接
    while (true)
    {
        // 尝试接受客户端连接
        auto client = m_server_socket->accept();
        if (!client)
        {
            // 如果没有更多连接，则退出循环
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            DANEJOE_LOG_ERROR("default", "EpollEventLoop", "accept failed: {}", std::strerror(errno));
            break;
        }
        // 确保设置客户端套接字非阻塞
        bool is_set_non_blocking = client->set_non_blocking(true);
        if (!is_set_non_blocking)
        {
            DANEJOE_LOG_ERROR("default", "EpollEventLoop", "set non blocking failed");
            client->close();
            continue;
        }
        // 确保在处理新连接时仅关注可读事件，而不是可写事件。
        // if (!add_socket(std::move(client), EventType::Readable | EventType::Writable | EventType::EdgeTriggered | EventType::PeerClosed))
        if (!add_socket(std::move(client), EventType::Readable | EventType::Writable | EventType::EdgeTriggered | EventType::PeerClosed))
        {
            DANEJOE_LOG_ERROR("default", "EpollEventLoop", "add socket failed");
            continue;
        }
        DANEJOE_LOG_TRACE("default", "EpollEventLoop", "add socket success");
        continue;
    }
}

void EpollEventLoop::readable_event(int32_t socket_id)
{
    // 检查当前的映射表中是否存在该socket
    auto socket_iter = m_sockets.find(socket_id);
    if (socket_iter == m_sockets.end())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "socket not found");
        return;
    }
    // 检查套接字是否有效
    if (socket_iter->second == nullptr)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "socket is null");
        remove_socket(PosixClientSocket::get_id(socket_id));
        return;
    }
    // 获取接收缓冲区
    auto buffer = m_recv_buffers.at(socket_id);
    // 从套接字中读取当前所有数据
    std::vector<uint8_t> data = m_sockets.at(socket_id)->read_all();
    if (!data.empty())
    {
        DANEJOE_LOG_TRACE("default", "EpollEventLoop", "From socket: {}, data size: {}", socket_id, data.size());
    }
    // 检查读取的长度
    uint32_t data_length = data.size();
    // 记录完成入队的数据
    uint32_t pushed_count = 0;
    while (pushed_count < data_length)
    {
        // 获取当前缓冲区剩余的空间长度
        uint32_t rest_space = buffer->get_max_size() - buffer->size();
        if (rest_space == 0)
        {
            DANEJOE_LOG_WARN("default", "EpollEventLoop", "buffer is full");
            /// @todo 后续考虑加入超时机制
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        // 获取当前可实际插入的数据长度
        uint32_t push_count = std::min(data_length - pushed_count, rest_space);
        // 入队数据
        buffer->push(data.begin() + pushed_count, data.begin() + push_count + pushed_count);
        pushed_count += push_count;
    }
#ifndef USE_SERVER_TRANS
    m_contexts.at(PosixClientSocket::get_id(socket_id))->on_recv();
#endif

#ifdef MODIFY_SOCKET_EVENT
    // 构建事件结构体
    struct epoll_event ev = {};
    // 监听可写事件
    ev.events = EPOLLOUT | EPOLLIN;
    ev.data.fd = socket_id;
    // 检查是否修改成功
    int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, socket_id, &ev);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "Failed to epoll_ctl: {}", std::strerror(errno));
    }
#endif
}

void EpollEventLoop::writable_event(int32_t socket_id)
{
    /// @brief 当前posix实现下，socket_map的键就是(int)fd
    auto client_iter = m_sockets.find(socket_id);
    // 检查当前映射表中是否存在记录
    if (client_iter == m_sockets.end())
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "socket_map not found!");
        return;
    }
    // 检查当前映射表中的socket对象是否有效
    if (!client_iter->second || !client_iter->second->is_valid())
    {
        remove_socket(socket_id);
        return;
    }
#ifndef USE_SERVER_TRANS
    // 暂时主动调用回调
    m_contexts.at(socket_id)->on_send();
#endif


    // 获取发送缓存
    auto buffer = m_send_buffers.at(socket_id);

    // 检查发送缓冲区数据的长度
    uint32_t data_length = buffer->size();
    auto data_optional = buffer->pop(data_length);

    if (data_optional.has_value())
    {
        std::string send_data = std::string(data_optional.value().begin(), data_optional.value().end());
        if (!send_data.empty() && send_data.length() != 4)
        {
            DANEJOE_LOG_TRACE("default", "EpollEventLoop", "To socket: {}, data size: {}", socket_id, send_data.size());
        }
        // 发送数据
        m_sockets.at(socket_id)->write_all(data_optional.value());
    }
#ifdef MODIFY_SOCKET_EVENT
    // 写完后修改监听事件
    struct epoll_event ev = {};
    // 监听可读事件
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = socket_id;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, socket_id, &ev) == -1)
    {
        DANEJOE_LOG_ERROR("default", "EpollEventLoop", "epoll_ctl failed!");
    }
#endif
}
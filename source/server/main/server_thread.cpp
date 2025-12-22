#include <danejoe/logger/logger_manager.hpp>

#include "danejoe/network/socket/posix_server_socket.hpp"
#include "danejoe/network/event_loop/epoll_event_loop.hpp"
#include "danejoe/network/context/i_socket_context.hpp"

#include "server/connect/trans_context.hpp"
#include "server/main/server_thread.hpp"

ServerThread::ServerThread(QObject* parent) : QThread(parent)
{}

ServerThread::~ServerThread() = default;

void ServerThread::run()
{
    /// @todo 设置地址复用，后续再抽象拓展
    int32_t opt_val = 1;
    DaneJoe::ISocketOption option;
    option.level = SOL_SOCKET;
    option.opt_name = SO_REUSEADDR | SO_REUSEPORT;
    option.opt_value = &opt_val;
    option.opt_length = sizeof(opt_val);

    // 创建服务器套接字
    std::unique_ptr<DaneJoe::PosixServerSocket> server = std::make_unique<DaneJoe::PosixServerSocket>("0.0.0.0", 8080, option);

    if (server->is_valid())
    {
        DANEJOE_LOG_TRACE("default", "server", "Server socket create success");
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "server", "Server socket create failed");
        return;
    }
    // 创建上下文创建者
#if PROTOCOL_TYPE == DANEJOE_PROTOCOL
    std::unique_ptr<DaneJoe::ISocketContextCreator> context_creator = std::make_unique<TransContextCreator>();
#elif PROTOCOL_TYPE == HTTP_PROTOCOL
    std::unique_ptr<DaneJoe::ISocketContextCreator> context_creator = std::make_unique<HttpContextCreator>();
#endif
    // 创建事件循环
    DaneJoe::EpollEventLoop loop(std::move(server), std::move(context_creator));
#ifdef USE_SERVER_TRANS
    ServerTrans::get_instance().start_loop();
#endif
    // 启动循环
    loop.run();
}

void ServerThread::stop()
{
    DANEJOE_LOG_TRACE("default", "server", "Server is stopping...");
    DaneJoe::EpollEventLoop::end_loop();
}
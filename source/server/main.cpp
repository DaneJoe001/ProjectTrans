#include <thread>
#include <chrono>
#include <fstream>

#include "log/manage_logger.hpp"
#include "network/posix_server_socket.hpp"
#include "network/posix_client_socket.hpp"
#include "network/epoll_event_loop.hpp"
#include "server/trans_context.hpp"
#include "database/sqlite_database.hpp"

using namespace std::literals::chrono_literals;

#define USE_EPOLL

#ifndef USE_EPOLL
int main(int argc, char** argv)
{
    std::ofstream log_file("./log/default.log", std::ios::out);
    log_file.clear();
    log_file.close();
    DANEJOE_LOG_TRACE("default", "Server", "Hello,world!");

    /// @brief 设置地址复用，后续再抽象拓展
    int opt_val = 1;
    ISocket::IOption option;
    option.level = 1;
    option.opt_name = 2;
    option.opt_val = &opt_val;
    option.opt_len = sizeof(opt_val);

    PosixServerSocket server("0.0.0.0", 8080, option);
    bool res = server.set_non_blocking(true);
    if (!res)
    {
        DANEJOE_LOG_INFO("default", "Server", "Failed to set nonblocking");
        return -1;
    }
    std::unique_ptr<IClientSocket> client;
    while (true)
    {
        client = server.accept();
        if (client && client->is_valid())
        {
            client->set_non_blocking(true);
            break;
        }
        else
        {
            DANEJOE_LOG_TRACE("default", "Server", "Waiting for connection");
            std::this_thread::sleep_for(1000ms);
        }
    }
    while (true)
    {
        if (!client->is_valid())
        {
            DANEJOE_LOG_TRACE("default", "Server", "Invalid client socket");
            return -1;
        }
        // DANEJOE_LOG_TRACE("default", "Server", "Try to read all!");
        std::vector<uint8_t> data = client->read_all();
        if (data.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        else
        {
            DANEJOE_LOG_INFO("default", "Server", "Recieved: {}", std::string(data.begin(), data.end()));
            client->send_all(data);
        }
    }
    return 0;
}

#endif

int main(int argc, char** argv)
{
    IDatabase::DatabaseConfig config;
    config.database_name = "server_database";
    config.path = "./database/server/server_database.db";
    DatabaseSQLite db;
    db.set_config(config);
    db.connect();
    std::ofstream log_file("./log/default.log", std::ios::out);
    log_file.clear();
    log_file.close();
    DANEJOE_LOG_TRACE("default", "Server", "Hello,world!");

    /// @brief 设置地址复用，后续再抽象拓展
    int opt_val = 1;
    ISocket::IOption option;
    option.level = 1;
    option.opt_name = 2;
    option.opt_val = &opt_val;
    option.opt_len = sizeof(opt_val);

    std::unique_ptr<PosixServerSocket> server = std::make_unique<PosixServerSocket>("0.0.0.0", 8080, option);
    std::unique_ptr<ISocketContextCreator> context_creator = std::make_unique<TransContextCreator>();
    TransContext context;
    EpollEventLoop loop(std::move(server), std::move(context_creator));
    loop.run();
    return 0;
}
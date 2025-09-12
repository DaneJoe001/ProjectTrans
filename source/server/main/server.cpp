#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <iostream>

#include "server/main/server.hpp"
#include "common/database/database_manager.hpp"
#include "common/database/sqlite_database.hpp"
#include "server/repository/file_info_repository.hpp"
#include "log/manage_logger.hpp"
#include "common/network/posix_server_socket.hpp"
#include "common/network/posix_client_socket.hpp"
#include "common/network/epoll_event_loop.hpp"
#include "server/connect/trans_context.hpp"
#include "server/view/server_main_window.hpp"

namespace fs = std::filesystem;

std::atomic<bool> g_is_server_running = false;
std::atomic<bool> g_is_panel_running = false;

void clear_log()
{
    std::ofstream log_file("./log/default.log", std::ios::out);
    log_file.clear();
    log_file.close();
}

void init_database()
{
    IDatabase::DatabaseConfig config;
    config.database_name = "server_database";
    config.path = "./database/server/server_database.db";
    DatabaseManager& database_manager = DatabaseManager::get_instance();
    database_manager.add_database("server_database", "sqlite");
    auto db = database_manager.get_database("server_database");
    db->set_config(config);
    db->connect();
    FileInfoRepository file_info_repository;
    file_info_repository.init();
    bool is_exist = file_info_repository.ensure_table_exists();
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "client", "Failed to create table file_info");
    }
}

void clear_database()
{
    fs::path path("./database/server/server_database.db");
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}

void run_server()
{
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
}

void stop_server_handler(int signal)
{
    DANEJOE_LOG_TRACE("default", "server", "Server is stopping...");
    g_is_server_running.store(false);
    /// @todo 后续再考虑安全改进
    exit(0);
}

void open_window_handler(int signal)
{
    g_is_panel_running.store(true);
}
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <iostream>

#include "server/main/server.hpp"
#include "common/database/database_manager.hpp"
#include "common/database/sqlite_database.hpp"
#include "server/repository/server_file_info_repository.hpp"
#include "log/manage_logger.hpp"
#include "common/network/posix_server_socket.hpp"
#include "common/network/posix_client_socket.hpp"
#include "common/network/epoll_event_loop.hpp"
#include "server/connect/trans_context.hpp"
#include "server/view/server_main_window.hpp"
#include "server/connect/server_trans.hpp"

#define USE_SERVER_TRANS

namespace fs = std::filesystem;

std::atomic<bool> g_is_panel_running = false;

void set_logger()
{
    DaneJoe::ILogger::LoggerConfig logger_config;
    logger_config.log_path = "./log/server.log";
    DaneJoe::ILogger::LogOutputSetting output_setting;
    output_setting.enable_function_name = true;
    output_setting.enable_line_num = true;
    output_setting.enable_file_name = true;
    auto logger = DaneJoe::ManageLogger::get_instance().get_logger("default");
    logger->set_config(logger_config);
    logger->set_output_settings(output_setting);
}

void clear_log()
{
    // 打开并清理文件
    std::ofstream log_file("./log/server.log", std::ios::trunc);
    log_file.close();
}

void init_database()
{
    // 设置数据库配置
    IDatabase::DatabaseConfig config;
    config.database_name = "server_database";
    config.path = "./database/server/server_database.db";
    // 获取数据库管理器实例
    DatabaseManager& database_manager = DatabaseManager::get_instance();
    // 添加数据库
    database_manager.add_database("server_database", "sqlite");
    // 获取数据库
    auto db = database_manager.get_database("server_database");
    // 设置数据库配置
    db->set_config(config);
    // 连接数据库
    db->connect();
    // 初始化文件信息仓库
    ServerFileInfoRepository file_info_repository;
    file_info_repository.init();
    // 确保对应表存在
    bool is_exist = file_info_repository.ensure_table_exists();
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "Server", "Failed to create table file_info");
    }
}

void clear_database()
{
    // 删除数据库文件
    fs::path path("./database/server/server_database.db");
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}

void run_server()
{
    /// @todo 设置地址复用，后续再抽象拓展
    int32_t opt_val = 1;
    ISocket::IOption option;
    option.level = 1;
    option.opt_name = 2;
    option.opt_val = &opt_val;
    option.opt_len = sizeof(opt_val);

    // 创建服务器套接字
    std::unique_ptr<PosixServerSocket> server = std::make_unique<PosixServerSocket>("0.0.0.0", 8080, option);
    // 创建上下文创建者
    std::unique_ptr<ISocketContextCreator> context_creator = std::make_unique<TransContextCreator>();
    // 创建事件循环
    EpollEventLoop loop(std::move(server), std::move(context_creator));
#ifdef USE_SERVER_TRANS
    ServerTrans::get_instance().start_loop();
#endif
    // 启动循环
    loop.run();
}

void stop_server_handler(int32_t signal)
{
    DANEJOE_LOG_TRACE("default", "server", "Server is stopping...");
#ifdef USE_SERVER_TRANS
    EpollEventLoop::end_loop();
#endif
    ServerTrans::get_instance().stop_loop();
    /// @todo 后续再考虑安全改进
    exit(0);
}

void open_window_handler(int32_t signal)
{
    g_is_panel_running.store(true);
}
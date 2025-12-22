#include <memory>
#include <filesystem>

#include <danejoe/database/sql_config.hpp>
#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "client/repository/client_file_info_repository.hpp"
#include "client/repository/block_request_info_repository.hpp"
#include "client/view/client_main_window.hpp"
#include "client/main/client_app.hpp"

namespace fs = std::filesystem;

ClientApp::ClientApp(QObject* parent) : QObject(parent)
{}
ClientApp::~ClientApp()
{
    if (m_main_window)
    {
        m_main_window->deleteLater();
    }
}

void ClientApp::init()
{
    init_logger();
    // 当需要时清理数据库
    // clear_database();
    // 初始化数据库
    init_database();
    // 清理日志
    clear_log();
    m_main_window = new ClientMainWindow();
    m_main_window->init();
    m_is_init = true;
}
void ClientApp::show_main_window()
{
    if (!m_is_init)
    {
        return;
    }
    m_main_window->show();
}
void ClientApp::init_database()
{
    // 创建数据库配置
    DaneJoe::SqlConfig config;
    // 设置数据库名称用于标识
    config.database_name = "client_database";
    // 设置数据库路径
    config.path = "./database/client/client_database.db";
    // 获取数据库管理器实例
    DaneJoe::SqlDatabaseManager& database_manager =
        DaneJoe::SqlDatabaseManager::get_instance();
    auto sqlite_driver = std::make_shared<DaneJoe::SqliteDriver>();
    // 添加Sqlite数据库
    database_manager.add_database("client_database", sqlite_driver);
    // 获取对应数据库
    auto db = database_manager.get_database("client_database");
    if (!db)
    {
        DANEJOE_LOG_ERROR("default", "Client", "Failed to get database client_database");
        return;
    }
    // 设置数据库配置
    db->set_config(config);
    // 连接数据库
    db->connect();
    // 创建客户端文件信息仓库
    ClientFileInfoRepository file_info_repository;
    // 初始化文件信息仓库
    file_info_repository.init();
    // 确保数据表存在，不存在则创建
    bool is_exist = file_info_repository.ensure_table_exists();
    // 判断是否已准备好数据表
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "Client", "Failed to create table file_info");
        return;
    }
    // 创建块请求信息仓库
    BlockRequestInfoRepository block_request_info_repository;
    // 初始化仓库
    block_request_info_repository.init();
    // 确保数据表存在，不存在则创建
    is_exist = block_request_info_repository.ensure_table_exists();
    // 判断是否已准备好数据表
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "Client", "Failed to create table block_request_info");
        return;
    }
}
void ClientApp::clear_database()
{
    // 获取客户端数据库路径
    /// @todo 考虑实现配置系统
    fs::path path = "./database/client/client_database.db";
    // 当路径存在时移除数据库
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}
void ClientApp::init_logger()
{
    DaneJoe::LoggerConfig logger_config;
    logger_config.log_path = "./log/client.log";
    DaneJoe::LogOutputSetting output_setting;
    output_setting.enable_function_name = true;
    output_setting.enable_line_num = true;
    output_setting.enable_time = false;
    // output_setting.enable_file_name = true;
    auto logger = DaneJoe::LoggerManager::get_instance().get_logger("default");
    logger->set_config(logger_config);
    logger->set_output_settings(output_setting);
}
void ClientApp::clear_log()
{
    // 打开并清理文件
    fs::path path = "./log/client.log";
    // 当路径存在时移除日志
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}
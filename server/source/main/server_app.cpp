#include <fstream>
#include <filesystem>
#include <iostream>
#include <QKeyEvent>

#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/logger/logger_config.hpp>
#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sql_config.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "repository/server_file_info_repository.hpp"
#include "main/server_app.hpp"
#include "view/widget/server_main_window.hpp"
#include "runtime/business_runtime.hpp"
#include "runtime/network_runtime.hpp"

namespace fs = std::filesystem;

ServerApp::ServerApp(QObject* parent) : QObject(parent)
{

}

ServerApp::~ServerApp()
{
    stop();
    if (m_server_main_window)
    {
        m_server_main_window->deleteLater();
    }
}

void ServerApp::init()
{
    // 初始化日志
    init_logger();
    // 清理数据库
    // clear_database();
    // 初始化数据库
    init_database();
    // 清理日志
    clear_log();
    m_server_main_window = new ServerMainWindow();
    m_server_main_window->init();

    m_reactor_mail_box =
        std::make_shared<DaneJoe::ReactorMailBox>();
    auto reactor_mail_box = m_reactor_mail_box;
    m_network_runtime =
        std::make_shared<NetworkRuntime>(m_reactor_mail_box);
    m_network_runtime->init();
    m_bussiness_runtime =
        std::make_shared<BusinessRuntime>(m_reactor_mail_box);
    m_bussiness_runtime->init();

    auto network_runtime = m_network_runtime;
    auto bussiness_runtime = m_bussiness_runtime;

    m_business_thread = std::thread([bussiness_runtime]()
        {
            if (bussiness_runtime)
            {
                bussiness_runtime->run();
            }
        });
    if (m_network_runtime && m_network_runtime->is_init())
    {
        m_network_thread = std::thread([network_runtime]()
            {
                if (network_runtime)
                {
                    network_runtime->run();
                }
            });
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "ServerApp", "Network runtime init failed: network thread not started");
    }
    m_is_init = true;
}

void ServerApp::stop()
{
    if (m_network_runtime)
    {
        m_network_runtime->stop();
    }
    if (m_bussiness_runtime)
    {
        m_bussiness_runtime->stop();
    }
    if (m_business_thread.joinable())
    {
        m_business_thread.join();
    }
    if (m_network_thread.joinable())
    {
        m_network_thread.join();
    }
}

bool ServerApp::eventFilter(QObject* watched, QEvent* event)
{
    if (event && event->type() == QEvent::KeyRelease)
    {
        auto* key_event = static_cast<QKeyEvent*>(event);
        if (!key_event->isAutoRepeat())
        {
            if (key_event->key() == Qt::Key_G
                && (key_event->modifiers() & Qt::ControlModifier)
                && (key_event->modifiers() & Qt::ShiftModifier))
            {
                if (m_is_main_window_visible)
                {
                    hide_main_window();
                }
                else
                {
                    show_main_window();
                }
                return true;
            }
            if (key_event->key() == Qt::Key_D
                && (key_event->modifiers() & Qt::ControlModifier)
                && (key_event->modifiers() & Qt::ShiftModifier))
            {

                return true;
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

void ServerApp::show_main_window()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ServerApp", "Failed to show main window: Server is not initialized");
        return;
    }
    DANEJOE_LOG_INFO("default", "ServerApp", "Show main window");
    m_server_main_window->show();
    m_is_main_window_visible = true;
}
void ServerApp::hide_main_window()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ServerApp", "Failed to hide main window: Server is not initialized");
        return;
    }
    DANEJOE_LOG_INFO("default", "ServerApp", "Hide main window");
    m_server_main_window->hide();
    m_is_main_window_visible = false;
}
void ServerApp::clear_log()
{
    // 打开并清理文件
    std::ofstream log_file("./log/server.log", std::ios::trunc);
    log_file.close();
}
void ServerApp::init_logger()
{
    DaneJoe::LoggerConfig logger_config;
    logger_config.log_path = "./log/server.log";
    logger_config.console_level = DaneJoe::LogLevel::DEBUG;
    DaneJoe::LogOutputSetting output_setting;
    output_setting.enable_function_name = true;
    output_setting.enable_line_num = true;
    output_setting.enable_time = false;
    // output_setting.enable_file_name = true;
    auto logger = DaneJoe::LoggerManager::get_instance().get_logger("default");
    logger->set_config(logger_config);
    logger->set_output_settings(output_setting);
}
void ServerApp::init_database()
{
    // 设置数据库配置
    DaneJoe::SqlConfig config;
    config.database_name = "server_database";
    config.path = "./database/server/server_database.db";
    // 获取数据库管理器实例
    auto& database_manager = DaneJoe::SqlDatabaseManager::get_instance();
    auto sqlite_driver = std::make_shared<DaneJoe::SqliteDriver>();
    // 添加数据库
    database_manager.add_database("server_database", sqlite_driver);
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
void ServerApp::clear_database()
{
    // 删除数据库文件
    fs::path path("./database/server/server_database.db");
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}
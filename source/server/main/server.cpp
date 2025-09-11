#include <fstream>

#include "server/main/server.hpp"
#include "common/database/database_manager.hpp"
#include "common/database/sqlite_database.hpp"
#include "server/repository/file_info_repository.hpp"
#include "log/manage_logger.hpp"

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
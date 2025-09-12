#include <filesystem>

#include "client/main/client.hpp"
#include "log/manage_logger.hpp"
#include "common/database/database_manager.hpp"
#include "client/repository/file_info_repository.hpp"
#include "client/repository/block_request_info_repository.hpp"

namespace fs = std::filesystem;

void init_database()
{
    IDatabase::DatabaseConfig config;
    config.database_name = "client_database";
    config.path = "./database/client/client_database.db";
    DatabaseManager& database_manager = DatabaseManager::get_instance();
    database_manager.add_database("client_database", "sqlite");
    auto db = database_manager.get_database("client_database");
    db->set_config(config);
    db->connect();
    FileInfoRepository file_info_repository;
    file_info_repository.init();
    bool is_exist = file_info_repository.ensure_table_exists();
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "client", "Failed to create table file_info");
    }
    BlockRequestInfoRepository block_request_info_repository;
    block_request_info_repository.init();
    is_exist = block_request_info_repository.ensure_table_exists();
    if (!is_exist)
    {
        DANEJOE_LOG_ERROR("default", "client", "Failed to create table block_request_info");
    }
}

void clear_database()
{
    fs::path path = "./database/client/client_database.db";
    if (fs::exists(path))
    {
        fs::remove(path);
    }
}
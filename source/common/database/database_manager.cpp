#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"
#include "common/database/sqlite_database.hpp"

DatabaseManager::DatabaseManager()
{
    // 添加sqlite作为默认数据库创建器
    add_creator("sqlite", std::make_shared<SQliteDatabaseCreator>());
}
DatabaseManager::~DatabaseManager() {}

DatabaseManager& DatabaseManager::get_instance()
{
    // 创建数据库管理器实例
    static DatabaseManager instance;
    // 返回实例
    return instance;
}
std::shared_ptr<IDatabase> DatabaseManager::get_database(const std::string& database_name)
{
    // 查找数据库并获取迭代器
    auto database_it = m_databases.find(database_name);
    // 没找到则返回空值
    if (database_it == m_databases.end())
    {
        DANEJOE_LOG_TRACE("default", "DatabaseManager", "Failed to find database: {}", database_name);
        return nullptr;
    }
    return database_it->second;
}
void DatabaseManager::add_database(const std::string& database_name, const std::string& database_type)
{
    // 先检查映射表中是否已存在该数据库
    auto database_it = m_databases.find(database_name);
    if (database_it != m_databases.end())
    {
        DANEJOE_LOG_INFO("default", "DatabaseManager", "Database {} already exisit!", database_name);
        return;
    }
    // 没有找到数据库后开始查找是否有对应创建器
    auto database_creator_it = m_database_creators.find(database_type);
    if (database_creator_it == m_database_creators.end())
    {
        DANEJOE_LOG_TRACE("default", "DatabaseManager", "Failed to find database creator: {}", database_type);
        return;
    }
    // 创建数据库
    m_databases[database_name] = database_creator_it->second->create_database();
}

void DatabaseManager::add_creator(const std::string& database_type, std::shared_ptr<IDatabaseCreator> database_creator)
{
    // 检查是否已存在对应创建器
    auto database_creator_it = m_database_creators.find(database_type);
    // 如果已存在则替换创建器
    if (database_creator_it != m_database_creators.end())
    {
        DANEJOE_LOG_INFO("default", "DatabaseManager", "Database creator {} will be replace!", database_type);
        database_creator_it->second = database_creator;
    }
    DANEJOE_LOG_TRACE("default", "DatabaseManager", "Add database creator: {}", database_type);
    // 添加新的创建器
    m_database_creators[database_type] = database_creator;
}
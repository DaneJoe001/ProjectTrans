#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"
#include "common/database/sqlite_database.hpp"

DatabaseManager::DatabaseManager()
{
    add_creator("sqlite", std::make_shared<SQliteDatabaseCreator>());
}
DatabaseManager::~DatabaseManager() {}

DatabaseManager& DatabaseManager::get_instance()
{
    static DatabaseManager instance;
    return instance;
}
std::shared_ptr<IDatabase> DatabaseManager::get_database(const std::string& database_name)
{
    if (m_databases.find(database_name) == m_databases.end())
    {
        DANEJOE_LOG_TRACE("default", "DatabaseManager", "Failed to find database: {}", database_name);
        return nullptr;
    }
    return m_databases[database_name];
}
void DatabaseManager::add_database(const std::string& database_name, const std::string& database_type)
{
    if (m_database_creators.find(database_type) == m_database_creators.end())
    {
        DANEJOE_LOG_TRACE("default", "DatabaseManager", "Failed to find database creator: {}", database_type);
    }
    m_databases[database_name] = m_database_creators[database_type]->create_database();
}

void DatabaseManager::add_creator(const std::string& database_type, std::shared_ptr<IDatabaseCreator> database_creator)
{
    m_database_creators[database_type] = database_creator;
}
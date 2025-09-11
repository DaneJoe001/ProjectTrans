#include <filesystem>

#include "common/database/sqlite_database.hpp"
#include "log/manage_logger.hpp"

// 确保 SQLite 命名空间的 Database 类已被正确前向声明或包含

namespace fs = std::filesystem;

DatabaseSQLite::~DatabaseSQLite()
{
}

bool DatabaseSQLite::connect()
{
    try
    {
        fs::path full_path = m_config.path;
        if (!fs::exists(full_path.parent_path()))
        {
            DANEJOE_LOG_TRACE("default", "Database", "Create file path: {}", m_config.path);
            fs::create_directories(full_path.parent_path());
        }
        m_database = std::make_unique<SQLite::Database>(m_config.path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        DANEJOE_LOG_ERROR("default", "Database", "Failed to connect to SQLite database: {}", e.what());
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
        throw;
    }
    return false;
}

bool DatabaseSQLite::execute(const std::string& statement)
{
    DANEJOE_LOG_DEBUG("default", "Database", "执行查询: {}", statement);
    try
    {
        if (!m_database)
        {
            DANEJOE_LOG_ERROR("default", "Database", "Database not connected");
            m_error_message = "Database not connected";
            m_error_code = "-1";
            return false;
        }
        DANEJOE_LOG_TRACE("default", "Database", "查询完毕 {}", statement);
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
        DANEJOE_LOG_ERROR("default", "Database", "error message: {}", m_error_message);
    }
    return false;
}

std::vector<std::vector<std::string>> DatabaseSQLite::query(const std::string& statement)
{
    std::vector<std::vector<std::string>> result;
    DANEJOE_LOG_TRACE("default", "Database", "执行查询 {}", statement);
    try
    {
        if (!m_database)
        {
            DANEJOE_LOG_ERROR("default", "Database", "Database not connected");
            m_error_message = "Database not connected";
            m_error_code = "-1";
            return result;
        }

        SQLite::Statement query(*m_database, statement);

        while (query.executeStep())
        {
            std::vector<std::string> row;
            for (int i = 0; i < query.getColumnCount(); i++)
            {
                row.push_back(query.getColumn(i).getString());
            }
            result.push_back(row);
        }
    }
    catch (const SQLite::Exception& e)
    {
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
    }
    DANEJOE_LOG_TRACE("default", "Database", "查询完毕 {}", statement);
    return result;
}

std::string DatabaseSQLite::error_message()
{
    return m_error_message;
}

std::string DatabaseSQLite::error_code()
{
    return m_error_code;
}

std::shared_ptr<IDatabase> SQliteDatabaseCreator::create_database()
{
    return std::make_shared<DatabaseSQLite>();
}
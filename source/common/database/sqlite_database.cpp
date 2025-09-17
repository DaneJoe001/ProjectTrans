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
        // 获取数据库目标路径
        fs::path full_path = m_config.path;
        // 检查父目录是否存在，不存在则创建目录
        if (!fs::exists(full_path.parent_path()))
        {
            DANEJOE_LOG_TRACE("default", "DatabaseSQLite", "Create file path: {}", m_config.path);
            fs::create_directories(full_path.parent_path());
        }
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            // 创建sqlite数据库句柄
            m_database = std::make_unique<SQLite::Database>(m_config.path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        }
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Failed to connect to SQLite database: {}", e.what());
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
        throw;
    }
    return false;
}

bool DatabaseSQLite::execute(const std::string& statement)
{
    try
    {
        // 检查数据库句柄是否有效
        if (!m_database)
        {
            DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Database not connected");
            m_error_message = "Database not connected";
            m_error_code = "-1";
            return false;
        }
        {
            // 执行查询
            std::lock_guard<std::mutex> lock(m_mutex);
            int32_t ret = m_database->exec(statement);
        }
    }
    catch (const SQLite::Exception& e)
    {
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "error message: {}", m_error_message);
    }
    DANEJOE_LOG_TRACE("default", "DatabaseSQLite", "查询完毕 {}", statement);
    return true;
}

std::vector<std::vector<std::string>> DatabaseSQLite::query(const std::string& statement)
{
    // 构建查询结果
    std::vector<std::vector<std::string>> result;
    try
    {
        // 检查数据库是否有效
        if (!m_database)
        {
            DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Database not connected");
            m_error_message = "Database not connected";
            m_error_code = "-1";
            return result;
        }
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            // 执行查询语句
            SQLite::Statement query(*m_database, statement);
            // 遍历查询结果
            while (query.executeStep())
            {
                // 获取每一行的数据
                std::vector<std::string> row;
                for (int32_t i = 0; i < query.getColumnCount(); i++)
                {
                    row.push_back(query.getColumn(i).getString());
                }
                result.push_back(row);
            }
        }
    }
    catch (const SQLite::Exception& e)
    {
        m_error_message = e.what();
        m_error_code = std::to_string(e.getErrorCode());
    }
    DANEJOE_LOG_TRACE("default", "DatabaseSQLite", "查询完毕 {}", statement);
    return result;
}

std::string DatabaseSQLite::error_message()
{
    // 返回错误消息
    return m_error_message;
}

std::string DatabaseSQLite::error_code()
{
    // 返回错误码
    return m_error_code;
}

std::shared_ptr<IDatabase> SQliteDatabaseCreator::create_database()
{
    // 创建SQlite数据库对象
    return std::make_shared<DatabaseSQLite>();
}
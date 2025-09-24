#include <filesystem>
#include <string>
#include <cstring>

#include "common/database/sqlite_database.hpp"
#include "log/manage_logger.hpp"

// 确保 SQLite 命名空间的 Database 类已被正确前向声明或包含

namespace fs = std::filesystem;

SQLiteStatement::SQLiteStatement(SQLiteDatabase* database, const std::string& statement)
{
    auto database_ptr = database->get_raw_database();
    if (!database_ptr)
    {
        return;
    }
    m_statement = std::make_unique<SQLite::Statement>(*database_ptr, statement);
}

bool SQLiteStatement::execute()
{
    if (!m_statement)
    {
        return false;
    }
    try
    {
        m_statement->exec();
    }
    catch (const SQLite::Exception& e)
    {
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Failed to execute: {}", e.what());
        return false;
    }
    return true;
}

std::vector<std::vector<SQLiteStatement::DataType>> SQLiteStatement::query()
{
    if (!m_statement)
    {
        return std::vector<std::vector<DataType>>();
    }
    std::vector<std::vector<DataType>> result;
    try
    {
        while (m_statement->executeStep())
        {
            int column_count = m_statement->getColumnCount();
            std::vector<DataType> row;
            row.reserve(column_count);
            for (int i = 0; i < column_count; i++)
            {
                auto data = m_statement->getColumn(i);
                auto type = data.getType();
                /// @todo 由于暂时未找到对应枚举，暂时以int代替枚举
                switch (type)
                {
                case 0:
                    row.emplace_back(nullptr);
                    break;
                case 1:
                    row.emplace_back(data.getInt64());
                    break;
                case 2:
                    row.emplace_back(data.getDouble());
                    break;
                case 3:
                    row.emplace_back(data.getText());
                    break;
                case 4:
                {
                    const void* blob = data.getBlob();
                    size_t blob_size = data.getBytes();
                    std::vector<uint8_t> blob_data(blob_size);
                    std::memcpy(blob_data.data(), blob, blob_size);
                    row.emplace_back(blob_data);
                }
                break;
                default:
                    // 处理未知类型
                    break;
                }
            }
            result.push_back(row);
        }
    }
    catch (const SQLite::Exception& e)
    {
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Failed to query: {}", e.what());
        return std::vector<std::vector<DataType>>();
    }
    return result;
}

void SQLiteStatement::bind(const DataType& value)
{
    if (!m_statement)
    {
        return;
    }
    try
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>)
            {
                m_statement->bind(static_cast<int>(m_param_index));
            }
            else if constexpr (std::is_same_v<T, int64_t>)
            {
                m_statement->bind(static_cast<int>(m_param_index), arg);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                m_statement->bind(static_cast<int>(m_param_index), arg);
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                m_statement->bind(static_cast<int>(m_param_index), arg);
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
            {
                m_statement->bind(static_cast<int>(m_param_index), arg.data(), static_cast<int>(arg.size()));
            }
            }, value);
        m_param_index++;
    }
    catch (const SQLite::Exception& e)
    {
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "Failed to bind parameter: {}", e.what());
    }
}

SQLiteDatabase::~SQLiteDatabase()
{
}

SQLite::Database* SQLiteDatabase::get_raw_database()
{
    return m_database.get();
}

std::unique_ptr<IStatement> SQLiteDatabase::get_statement(const std::string statement)
{
    return std::make_unique<SQLiteStatement>(this, statement);
}
bool SQLiteDatabase::connect()
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
    }
    return false;
}

bool SQLiteDatabase::execute(const std::string& statement)
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
        DANEJOE_LOG_ERROR("default", "DatabaseSQLite", "error message: {},statement: {}", m_error_message, statement);
        return false;
    }
    DANEJOE_LOG_TRACE("default", "DatabaseSQLite", "查询完毕 {}", statement);
    return true;
}

std::vector<std::vector<std::string>> SQLiteDatabase::query(const std::string& statement)
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

std::string SQLiteDatabase::error_message()
{
    // 返回错误消息
    return m_error_message;
}

std::string SQLiteDatabase::error_code()
{
    // 返回错误码
    return m_error_code;
}

std::shared_ptr<IDatabase> SQliteDatabaseCreator::create_database()
{
    // 创建SQlite数据库对象
    return std::make_shared<SQLiteDatabase>();
}
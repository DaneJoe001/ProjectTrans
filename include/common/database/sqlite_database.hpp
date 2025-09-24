#pragma once

/**
 * @file sqlite_database.hpp
 * @brief SQLite数据库
 * @author DaneJoe001
 */

#include <memory>
#include <mutex>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include "common/database/i_database.hpp"

class SQLiteDatabase;

class SQLiteStatement :public IStatement
{
public:
    struct ColumnData
    {
        SQLite::Column m_column;
        ColumnData(SQLite::Column column) :m_column(column)
        {
        }
    };
public:
    SQLiteStatement(SQLiteDatabase* database, const std::string& statement);
    void bind(const DataType& value)override;
    bool execute()override;
    std::vector<std::vector<DataType>> query()override;
private:
    uint32_t m_param_index = 1;
    std::unique_ptr<SQLite::Statement> m_statement = nullptr;
};

/**
 * @class DatabaseSQLite
 * @brief SQLite数据库
 */
class SQLiteDatabase : public IDatabase
{
public:
    /**
     * @brief 构造函数
     */
    SQLiteDatabase() = default;
    /**
     * @brief 析构函数
     */
    ~SQLiteDatabase();
    /**
     * @brief 连接数据库
     */
    virtual bool connect() override;
    /**
     * @brief 执行SQL语句
     */
    virtual bool execute(const std::string& statement) override;
    /**
     * @brief 查询SQL语句
     */
    virtual std::vector<std::vector<std::string>> query(const std::string& statement) override;
    /**
     * @brief 获取错误信息
     */
    virtual std::string error_message() override;
    /**
     * @brief 获取错误码
     */
    virtual std::string error_code() override;
    SQLite::Database* get_raw_database();
    std::unique_ptr<IStatement> get_statement(const std::string statement)override;
private:
    /// @brief 数据库连接
    std::unique_ptr<SQLite::Database> m_database;
    /// @brief 互斥锁
    std::mutex m_mutex;
};

/**
 * @class SQliteDatabaseCreator
 * @brief SQLite数据库创建器
 */
class SQliteDatabaseCreator :public IDatabaseCreator
{
public:
    /**
     * @brief 创建数据库
     * @return 数据库
     */
    std::shared_ptr<IDatabase> create_database()override;
};
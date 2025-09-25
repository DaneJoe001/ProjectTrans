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

/**
 * @class SQLiteStatement
 * @brief SQLite语句
 */
class SQLiteStatement :public IStatement
{
private:
    /**
     * @brief 列数据
     */
    struct ColumnData
    {
        SQLite::Column m_column;
        ColumnData(SQLite::Column column) :m_column(column)
        {
        }
    };
public:
    /**
     * @brief 构造函数
     * @param database 数据库
     * @param statement 语句
     */
    SQLiteStatement(SQLiteDatabase* database, const std::string& statement);
    /**
     * @brief 绑定参数
     * @param value 参数值
     */
    void bind(const DataType& value)override;
    /**
     * @brief 执行语句
     * @return 是否执行成功
     */
    bool execute()override;
    /**
     * @brief 查询数据
     * @return 数据
     */
    std::vector<std::vector<DataType>> query()override;
private:
    /// @brief 参数索引
    uint32_t m_param_index = 1;
    /// @brief 语句
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
    /**
     * @brief 获取原始数据库
     * @return 原始数据库
     */
    SQLite::Database* get_raw_database();
    /**
     * @brief 获取语句
     * @param statement 语句
     * @return 语句
     */
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
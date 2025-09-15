#pragma once

/**
 * @file sqlite_database.hpp
 * @brief SQLite数据库
 * @author DaneJoe001
 */

#include <memory>
#include <mutex>

#include <SQLiteCpp/SQLiteCpp.h>

#include "common/database/i_database.hpp"

/**
 * @class DatabaseSQLite
 * @brief SQLite数据库
 */
class DatabaseSQLite : public IDatabase
{
public:
    /**
     * @brief 构造函数
     */
    DatabaseSQLite() = default;
    /**
     * @brief 析构函数
     */
    ~DatabaseSQLite();
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
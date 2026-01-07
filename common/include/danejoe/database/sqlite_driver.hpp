/**
 * @file sqlite_driver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief SQLite 驱动
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义基于 SQLite3 的数据库驱动实现 SqliteDriver。
 *          该驱动实现 ISqlDriver 接口，并负责 sqlite3/sqlite3_stmt 资源管理与预编译语句缓存。
 */
#pragma once

#include <memory>
#include <unordered_map>

#include "danejoe/database/i_sql_driver.hpp"

 // #include "danejoe/database/sql_database.hpp"

extern "C"
{
#include <sqlite3.h>
}

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @struct SqliteStmtDeleter
     * @brief SQLite 语句删除器
     * @details 用于配合 std::unique_ptr 管理 sqlite3_stmt 的生命周期。
     */
    struct SqliteStmtDeleter
    {
        /**
         * @brief 删除 SQLite 语句
         * @param stmt SQLite 语句
         * @details 释放 sqlite3_stmt 对象。
         */
        void operator()(sqlite3_stmt* stmt);
    };
    /// @brief SQLite 语句指针(unique_ptr)
    using SqliteStmtPtr = std::unique_ptr<sqlite3_stmt, SqliteStmtDeleter>;
    /**
     * @class SqliteDriver
     * @brief SQLite 驱动
     * @details SQLite3 的 ISqlDriver 实现。
     *          - connect() 打开数据库连接
     *          - prepare() 预编译 SQL 并缓存 sqlite3_stmt
     *          - bind/reset/clear_bindings/execute_* 通过 SQL 字符串索引缓存的预编译语句
     * @note SQLite 参数索引通常从 1 开始；若上层使用 0 基索引，需由实现进行适配。
     * @note 线程安全性取决于 SQLite 编译选项与该类实现；默认不建议多线程并发调用同一实例。
     */
    class SqliteDriver : public ISqlDriver
    {
    public:
        /**
         * @brief 构造函数
         */
        SqliteDriver();
        /**
         * @brief 析构函数
         */
        ~SqliteDriver();
        /**
         * @brief 连接数据库
         * @param config 数据库配置
         * @return true 连接成功
         * @return false 连接失败
         */
        bool connect(const SqlConfig& config)override;
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return true 准备成功
         * @return false 准备失败
         */
        bool prepare(const std::string& sql)override;
        /**
         * @brief 绑定SQL语句
         * @param sql SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return true 绑定成功
         * @return false 绑定失败
         */
        bool bind(const std::string& sql, int index, const SqlCell& cell)override;
        /**
         * @brief 重置SQL语句
         * @param sql SQL语句
         */
        void reset(const std::string& sql)override;
        /**
         * @brief 清除SQL语句绑定
         * @param sql SQL语句
         */
        void clear_bindings(const std::string& sql)override;
        /**
         * @brief 执行查询
         * @param sql SQL语句
         * @return 查询结果
         */
        std::vector<std::vector<SqlCell>> execute_query(const std::string& sql)override;
        /**
         * @brief 执行命令
         * @param sql SQL语句
         * @return true 执行成功
         * @return false 执行失败
         */
        bool execute_command(const std::string& sql)override;
        /**
         * @brief 关闭数据库
         */
        void close()override;
    private:
        /// @brief Sqlite 句柄指针
        sqlite3* m_db = nullptr;
        /// @brief 准备语句映射
        std::unordered_map<std::string, SqliteStmtPtr> m_stmts;
    };

}

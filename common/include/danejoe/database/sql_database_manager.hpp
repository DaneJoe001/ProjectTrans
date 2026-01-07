/**
 * @file sql_database_manager.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库管理器
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlDatabaseManager，用于集中管理 SqlDatabase 实例的创建与访问。
 *          通过 add_database() 注册数据库实例（或其驱动），并可通过名称获取对应的 SqlDatabase。
 */
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "danejoe/database/sql_database.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlDatabaseManager
     * @brief 数据库管理器
     * @details 单例管理器，以名称为键维护多个数据库对象。
     * @note 线程安全性由具体实现决定；除非实现内部做了同步，否则应避免并发 add/get 操作。
     */
    class SqlDatabaseManager
    {
    public:
        /**
         * @brief 获取实例
         * @return 实例
         * @details 返回全局唯一实例。
         */
        static SqlDatabaseManager& get_instance();
        /**
         * @brief 获取数据库
         * @param database_name 数据库名称
         * @return 数据库
         * @details 根据名称获取已注册的数据库对象；若不存在则返回空指针或创建新实例的行为由实现决定。
         */
        std::shared_ptr<SqlDatabase> get_database(const std::string& database_name);
        /**
         * @brief 添加数据库
         * @param database_name 数据库名称
         * @param driver 数据库驱动
         * @details 将 driver 关联到指定名称，并注册对应的 SqlDatabase 实例。
         */
        void add_database(
            const std::string& database_name, 
            std::shared_ptr<ISqlDriver> driver);
    private:
        /**
         * @brief 构造函数
         */
        SqlDatabaseManager();
        /**
         * @brief 析构函数
         */
        ~SqlDatabaseManager();
    private:
        /// @brief 数据库
        std::unordered_map<std::string, std::shared_ptr<SqlDatabase>> m_databases;
    };
}

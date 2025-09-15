#pragma once

/**
 * @file database_manager.hpp
 * @brief 数据库管理器
 * @author DaneJoe001
 */

#include <memory>
#include <string>
#include <unordered_map>

#include "common/database/i_database.hpp"

/**
 * @class DatabaseManager
 * @brief 数据库管理器
 */
class DatabaseManager
{
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
    static DatabaseManager& get_instance();
    /**
     * @brief 获取数据库
     * @param database_name 数据库名称
     * @return 数据库
     */
    std::shared_ptr<IDatabase> get_database(const std::string& database_name);
    /**
     * @brief 添加数据库
     * @param database_name 数据库名称
     * @param database_type 数据库类型
     */
    void add_database(const std::string& database_name, const std::string& database_type);
    /**
     * @brief 添加数据库创建器
     * @param database_type 数据库类型
     * @param database_creator 数据库创建器
     */
    void add_creator(const std::string& database_type, std::shared_ptr<IDatabaseCreator> database_creator);
private:
    /**
     * @brief 构造函数
     */
    DatabaseManager();
    /**
     * @brief 析构函数
     */
    ~DatabaseManager();
private:
    /// @brief 数据库
    std::unordered_map<std::string, std::shared_ptr<IDatabase>> m_databases;
    /// @brief 数据库创建器
    std::unordered_map<std::string, std::shared_ptr<IDatabaseCreator>> m_database_creators;
};
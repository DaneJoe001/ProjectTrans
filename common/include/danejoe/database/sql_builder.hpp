/**
 * @file sql_builder.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Sql语句构建器
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlBuilder，用于基于表结构信息（SqlTableItem）与字符串化策略（ISqlStringify）构建常见 SQL 语句。
 *          build_* 系列接口返回 std::nullopt 表示缺少必要上下文或当前实现无法完成字符串化。
 */
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "danejoe/database/sql_schema.hpp"
#include "danejoe/database/sql_condition.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlBuilder
     * @brief 数据库构建器
     * @details 负责拼接创建表、插入、查询、更新、删除等 SQL 字符串。
     *          在调用 build_* 之前，应先通过构造函数或 set_table_info()/set_stringify() 注入必要依赖。
     */
    class SqlBuilder
    {
    public:
        /**
         * @brief 默认构造函数
         * @details 需要后续调用 set_table_info() 与 set_stringify() 设置依赖后才能构建 SQL。
         */
        SqlBuilder();

        /**
         * @brief 设置表信息
         * @param table_info 表信息
         */
        void set_table_info(const std::shared_ptr<const SqlTableItem> table_info);
        /**
         * @brief 构造函数
         * @param table_info 表信息
         * @param sql_stringify 字符串化接口
         */
        SqlBuilder(
            const std::shared_ptr<const SqlTableItem> table_info,
            std::shared_ptr<ISqlStringify> sql_stringify);
        /**
         * @brief 设置字符串化接口
         * @param sql_data_type 字符串化接口
         * @details 用于将 DataType/条件等结构格式化为 SQL 片段。
         */
        void set_stringify(std::shared_ptr<ISqlStringify> sql_data_type);
        /**
         * @brief 构建创建表字符串
         * @return 字符串
         * @details 依赖 m_table_info 与 m_sql_stringify；若缺失依赖或遇到不支持的类型则返回 std::nullopt。
         */
        std::optional<std::string> build_create_table_string()const;
        /**
         * @brief 构建条件字符串
         * @param condition_items 条件信息
         * @return 字符串
         * @details 将多个条件项拼接为 SQL 条件表达式。
         */
        std::optional<std::string> build_condition_string(const std::vector<SqlConditionItem>& condition_items)const;
        /**
         * @brief 构建插入字符串
         * @param dest_column_items 目标列信息
         * @return 字符串
         * @details 通常生成 INSERT 语句骨架（列列表与占位符/值部分由实现决定）。
         */
        std::optional<std::string> build_insert_string(const std::vector<SqlColumnItem>& dest_column_items)const;
        /**
         * @brief 构建查询字符串
         * @param dest_column_items 目标列信息
         * @param condition_items 条件信息
         * @return 字符串
         */
        std::optional<std::string> build_select_string(
            const std::vector<SqlColumnItem>& dest_column_items,
            const std::vector<SqlConditionItem>& condition_items)const;
        /**
         * @brief 构建更新字符串
         * @param dest_column_items 目标列信息
         * @param condition_items 条件信息
         * @param enable_all 是否允许无条件更新
         * @return 字符串
         * @details 默认不允许生成无 WHERE 的更新语句；当 condition_items 为空且 enable_all 为 false 时通常返回 std::nullopt。
         */
        std::optional<std::string> build_update_string(
            const std::vector<SqlColumnItem>& dest_column_items,
            const std::vector<SqlConditionItem>& condition_items, bool enable_all = false)const;
        /**
         * @brief 构建删除字符串
         * @param condition_items 条件信息
         * @param enable_all 是否启用全部
         * @return 字符串
         * @details 默认不允许生成无 WHERE 的删除语句；当 condition_items 为空且 enable_all 为 false 时通常返回 std::nullopt。
         */
        std::optional<std::string> build_delete_string(const std::vector<SqlConditionItem>& condition_items, bool
            enable_all = false)const;
        /**
         * @brief 构建计数查询字符串
         * @param condition_items 条件信息
         * @return 字符串
         */
        std::optional<std::string> build_count_string(const std::vector<SqlConditionItem>& condition_items)const;
    private:
        /**
         * @brief 构建目标列字符串
         * @param column_items 列信息
         * @return 字符串
         */
        std::optional<std::string> build_dest_columns_string(const std::vector<SqlColumnItem>& column_items)const;
    private:
        /// @brief 记录表信息
        std::shared_ptr<const SqlTableItem> m_table_info = nullptr;
        /// @brief 字符串化接口
        std::shared_ptr<ISqlStringify> m_sql_stringify = nullptr;
    };
}

/**
 * @file sql_table_query.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库表查询
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlTableQuery，用于面向单表进行常见 CRUD 操作。
 *          内部组合 SqlBuilder（生成 SQL）与 SqlQuery（绑定/执行），并依赖 ISqlStringify 提供字符串化策略。
 */
#pragma once

#include "danejoe/database/sql_query.hpp"
#include "danejoe/database/sql_builder.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlTableQuery
     * @brief 数据库表查询
     * @details 面向单表的查询/写入封装。
     *          在调用 create/insert/update/remove/select/count 前，应确保已设置：
     *          - 表结构信息（set_table_info）
     *          - 查询对象（set_query）
     *          - 字符串化接口（set_stringify 或通过 SqlBuilder 注入）
     * @note 线程安全性依赖底层 SqlQuery/驱动实现；默认不建议并发调用同一实例。
     */
    class SqlTableQuery
    {
    public:
        /**
         * @brief 默认构造函数
         */
        SqlTableQuery();
        /**
         * @brief 设置字符串化接口
         * @param stringify 字符串化接口
         */
        void set_stringify(std::shared_ptr<ISqlStringify> stringify);
        /**
         * @brief 设置表信息
         * @param table_info 表信息
         */
        void set_table_info(const std::shared_ptr<const SqlTableItem> table_info);
        /**
         * @brief 设置查询对象
         * @param query 查询对象
         */
        void set_query(SqlQueryPtr query);
        /**
         * @brief 构造函数
         * @param stringify 字符串化接口
         * @param table_info 表信息
         * @param query 查询
         */
        SqlTableQuery(
            std::shared_ptr<ISqlStringify> stringify,
            const std::shared_ptr<const SqlTableItem> table_info,
            SqlQueryPtr query);
        /**
         * @brief 创建表
         * @return true 创建成功
         * @return false 创建失败
         * @details 使用 SqlBuilder 根据表结构构建 CREATE TABLE 并通过 SqlQuery 执行。
         */
        bool create();
        /**
         * @brief 更新
         * @param cells 单元格
         * @param conditions 条件
         * @return true 更新成功
         * @return false 更新失败
         * @details 根据 cells 推导目标列，并构建 UPDATE 语句后执行。
         */
        bool update(
            const std::vector<SqlCell>& cells,
            const std::vector<SqlConditionItem>& conditions);
        /**
         * @brief 插入
         * @param cells 单元格
         * @return true 插入成功
         * @return false 插入失败
         * @details 根据 cells 推导目标列，并构建 INSERT 语句后执行。
         */
        bool insert(const std::vector<SqlCell>& cells);
        /**
         * @brief 删除
         * @param conditions 条件
         * @return true 删除成功
         * @return false 删除失败
         * @details 根据 conditions 构建 DELETE 语句后执行。
         */
        bool remove(const std::vector<SqlConditionItem>& conditions);
        /**
         * @brief 查询
         * @param dest_columns 目标列
         * @param conditions 条件
         * @return 查询结果
         * @details 构建 SELECT 语句并执行，返回二维结果集：外层为行，内层为列（SqlCell）。
         */
        std::vector<std::vector<SqlCell>> select(
            const std::vector<SqlColumnItem>& dest_columns,
            const std::vector<SqlConditionItem>& conditions);
        /**
         * @brief 计数
         * @param conditions 条件
         * @return 满足条件的记录数
         */
        int64_t count(const std::vector<SqlConditionItem>& conditions);
    private:
        /**
         * @brief 获取目标列
         * @param cells 单元格
         * @return 目标列
         * @details 根据输入的 cells 推导需要写入/更新的列集合。
         */
        std::vector<SqlColumnItem> get_dest_columns(const std::vector<SqlCell>& cells)const;
    private:
        /// @brief 表信息
        std::shared_ptr<const SqlTableItem> m_table_info = nullptr;
        /// @brief 构建器
        SqlBuilder m_builder;
        /// @brief 查询
        SqlQueryPtr m_query = nullptr;
    };
}

/**
 * @file sql_schema.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库表结构
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义数据库表结构描述：SqlColumnItem（列信息）与 SqlTableItem（表信息）。
 *          这些结构通常用于 SQL 构建器生成 CREATE TABLE/INSERT/SELECT 等语句。
 */
#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <string>

#include "danejoe/common/core/data_type.hpp"
#include "danejoe/database/sql_cell.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @struct SqlColumnItem
     * @brief 数据库列信息
     * @details 描述表中单个列的结构与约束：
     *          - column_index：列索引（可选，未设置时通常为 -1）
     *          - column_name：列名称
     *          - data_type：数据类型（DataType）
     *          - is_primary_key/is_not_null/is_unique：约束标记
     *          - default_value：可选默认值（以 CellData 表示）
     */
    struct SqlColumnItem
    {
        /// @brief 列索引
        int32_t column_index = -1;
        /// @brief 列名称
        std::string column_name = "";
        /// @brief 数据类型
        DaneJoe::DataType data_type;
        /// @brief 是否为主键
        bool is_primary_key = false;
        /// @brief 是否不为空
        bool is_not_null = true;
        /// @brief 是否唯一
        bool is_unique = false;
        /// @brief 默认值
        std::optional<DaneJoe::CellData> default_value;
        /**
         * @brief 是否有效
         * @return true 列信息有效
         * @return false 列信息无效
         */
        bool is_valid()const;
        /**
         * @brief 转换为字符串
         * @return 字符串
         */
        std::string to_string()const;
        /**
         * @brief 获取默认数据单元
         * @return 默认数据单元
         */
        SqlCell get_default_cell()const;
        /**
         * @brief 是否相等
         * @param other 其他列信息
         * @return true 相等
         * @return false 不相等
         */
        bool operator==(const SqlColumnItem& other)const;
    };
    /**
     * @struct SqlTableItem
     * @brief 数据库表信息
     * @details 描述表名称与列集合。
     *          - table_name：表名称
     *          - column_items：列信息列表
     */
    struct SqlTableItem
    {
        /// @brief 表名称
        std::string table_name;
        /// @brief 列信息
        std::vector<SqlColumnItem> column_items;
        /// @brief 是否唯一
        bool is_unique = false;
        /**
         * @brief 是否有效
         * @return true 表信息有效
         * @return false 表信息无效
         */
        bool is_valid()const;
        /**
         * @brief 是否包含列
         * @param column_name 列名称
         * @return true 包含该列
         * @return false 不包含该列
         */
        bool has_column(const std::string &column_name) const;

        /**
         * @brief 添加列
         * @param column 列信息
         */
        void add_column(const SqlColumnItem& column);
        /**
         * @brief 是否包含列
         * @param column 列信息
         * @return true 包含该列
         * @return false 不包含该列
         */
        bool has_column(const SqlColumnItem& column)const;
        /**
         * @brief 获取列信息
         * @param column_name 列名称
         * @return 列信息
         */
        std::optional<SqlColumnItem> get_column_info(const std::string& column_name)const;
    };
}

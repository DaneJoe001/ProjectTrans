/**
 * @file sqlite_stringify.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief SQLite字符串化接口
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqliteStringify，用于将 DataType 与 SqlConditionItem 等结构格式化为 SQLite 方言的 SQL 字符串片段。
 *          格式化失败或不支持的条件类型将返回 std::nullopt。
 */
#pragma once

#include "danejoe/condition/single_interval.hpp"
#include "danejoe/database/i_sql_stringify.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqliteStringify
     * @brief SQLite字符串化接口
     * @details ISqlStringify 的 SQLite 方言实现，用于生成适用于 SQLite 的类型声明与条件表达式。
     *          try_format_* 返回 std::nullopt 表示当前条件项无法转换为 SQL。
     */
    struct SqliteStringify : public ISqlStringify
    {
        /**
         * @brief 格式化数据类型
         * @param type 数据类型
         * @return 字符串
         * @details 将 DataType 映射为 SQLite 类型声明字符串。
         */
        std::optional<std::string> try_format_data_type(DataType type) const override;
        /**
         * @brief 构建条件字符串
         * @param item 条件信息
         * @return 字符串
         * @details 将条件项格式化为 SQLite 可执行的 WHERE/ORDER BY 片段。
         */
        std::optional<std::string> try_format_condition(const SqlConditionItem& item) const override;
        /**
         * @brief 构建范围条件字符串
         * @param item 条件信息
         * @return 字符串
         * @details 针对范围类条件（例如整数区间/区间集合）生成条件表达式。
         */
        std::optional<std::string> try_format_range_condition(const SqlConditionItem& item)const;
        /**
         * @brief 构建文本条件字符串
         * @param item 条件信息
         * @return 字符串
         * @details 针对文本匹配条件生成条件表达式（具体匹配规则由条件对象决定）。
         */
        std::optional<std::string> try_format_text_condition(const SqlConditionItem& item)const;
        /**
         * @brief 构建整数区间条件字符串
         * @param interval 区间
         * @param column_name 列名称
         * @return 字符串
         * @details 将单个整数区间格式化为 SQLite 条件表达式片段。
         */
        std::optional<std::string>  try_format_integer_interval(
            const SingleInterval<int64_t>& interval,
            const std::string& column_name)const;
    };
}

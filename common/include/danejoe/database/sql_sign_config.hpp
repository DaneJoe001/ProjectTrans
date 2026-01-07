/**
 * @file sql_sign_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 符号配置
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlSignConfig，用于配置 SQL 构建/字符串化过程中的关键符号与关键字（占位符、比较符、逻辑符等）。
 *          不同数据库方言可通过覆盖该配置实现差异化输出。
 */
#pragma once

#include <string>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 符号配置
     * @details 用于 SQL 字符串拼接的符号/关键字集合。
     *          默认值为常见 SQL 表达：
     *          - place_holder_sign 为 "?"（常见于 SQLite 等使用位置参数的后端）
     *          - or_sign/and_sign 为 "OR"/"AND"
     */
    struct SqlSignConfig
    {
        /// @brief 占位符符号
        std::string place_holder_sign = "?";
        /// @brief 等于符号
        std::string equal_sign = "=";
        /// @brief 小于符号
        std::string less_than_sign = "<";
        /// @brief 大于符号
        std::string greater_than_sign = ">";
        /// @brief 逗号符号
        std::string comma_sign = ",";
        /// @brief 空格符号
        std::string space_sign = " ";
        /// @brief OR符号
        std::string or_sign = "OR";
        /// @brief AND符号
        std::string and_sign = "AND";
        /// @brief 左括号
        std::string left_parenthesis = "(";
        /// @brief 右括号
        std::string right_parenthesis = ")";
    };
}

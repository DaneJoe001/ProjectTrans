/**
 * @file i_sql_stringify.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库字符串化接口
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 ISqlStringify，用于将数据库相关结构（数据类型、条件表达式等）格式化为 SQL 片段字符串。
 *          该接口仅负责字符串化与符号配置，不负责执行 SQL。
 */
#pragma once

#include <optional>
#include <string>
#include <chrono>

#include "danejoe/common/core/data_type.hpp"
#include "danejoe/database/sql_condition.hpp"
#include "danejoe/database/sql_sign_config.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISqlStringify
     * @brief 数据库字符串化接口
     * @details 用于将 DataType、SqlConditionItem 等结构转换为 SQL 字符串片段。
     *          try_format_* 返回 std::nullopt 表示当前实现不支持该类型/条件项的字符串化。
     *          可通过 set_sign_config() 注入符号配置（例如比较符号、逻辑符号等的定制）。
     */
    class ISqlStringify
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~ISqlStringify() {}
        /**
         * @brief 格式化数据类型
         * @param type 数据类型
         * @return 字符串
         * @details 将 DataType 转换为 SQL 类型声明字符串（例如 "INTEGER"、"TEXT" 等）。
         */
        virtual std::optional<std::string> try_format_data_type(DataType type) const = 0;
        /**
         * @brief 构建条件字符串
         * @param item 条件信息
         * @return 字符串
         * @details 将单个条件项格式化为 SQL 条件表达式片段（例如 "age >= 18"）。
         */
        virtual std::optional<std::string> try_format_condition(const SqlConditionItem& item)const = 0;
        /**
         * @brief 设置符号配置
         * @param config 符号配置
         * @details 设置用于格式化 SQL 的符号/关键字配置。
         */
        void set_sign_config(const SqlSignConfig& config);
        /**
         * @brief 获取符号配置
         * @return 符号配置
         */
        SqlSignConfig get_sign_config();
    protected:
        /// @brief 符号配置
        SqlSignConfig m_sign_config;
    };

    /**
     * @brief 将时间点转换为毫秒
     * @param time system_clock 时间点
     * @return 以毫秒表示的时间戳
     */
    int64_t to_time_ms(const std::chrono::system_clock::time_point& time);

    /**
     * @brief 将时间点转换为毫秒
     * @param time steady_clock 时间点
     * @return 以毫秒表示的时间戳
     * @note steady_clock 的返回值仅适用于相对时间/持续时间计算，不应作为跨进程/跨机器的绝对时间戳使用。
     */
    int64_t to_time_ms(const std::chrono::steady_clock::time_point& time);
    template<typename T>
    T from_time_ms(int64_t time) = delete;

    /**
     * @brief 从毫秒值恢复 system_clock 时间点
     * @param time 毫秒值
     * @return system_clock 时间点
     */
    template<>
    std::chrono::system_clock::time_point from_time_ms(int64_t time);

    /**
     * @brief 从毫秒值恢复 steady_clock 时间点
     * @param time 毫秒值
     * @return steady_clock 时间点
     * @note steady_clock 的时间点仅具备相对意义，恢复后的值仅应与同一时钟来源的值进行比较。
     */
    template<>
    std::chrono::steady_clock::time_point from_time_ms(int64_t time);
}

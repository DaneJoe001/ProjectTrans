/**
 * @file diagnostic_event.hpp
 * @brief 诊断事件
 * @author DaneJoe001
 * @note 诊断系统主要用于库内部使用，不对外作保证
 * @version 0.2.0
 * @date 2025-12-19
 */
#pragma once

#include <cstdint>
#include <string>
#include <chrono>

#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum DiagnosticEventLevel
     * @brief 诊断事件等级
     */
    enum class DiagnosticEventLevel
    {
        /// @brief 跟踪
        Trace,
        /// @brief 调试
        Debug,
        /// @brief 信息
        Info,
        /// @brief 警告
        Warn,
        /// @brief 错误
        Error,
        /// @brief 严重错误
        Critical,
        /// @brief 未知
        Unknown
    };
    /**
     * @brief 将诊断事件等级转换为字符串
     * @param level 诊断事件等级
     * @return 诊断事件等级字符串
     */
    std::string to_string(DiagnosticEventLevel level);

    /**
     * @struct DiagnosticEvent
     * @brief 诊断事件
     */
    struct DiagnosticEvent
    {
        /// @brief 事件标识符
        int64_t event_id;
        /// @brief 线程标识符（由线程 id 哈希得到）
        int64_t thread_id;
        /// @brief 事件等级
        DiagnosticEventLevel level;
        /// @brief 模块名称
        std::string module_name;
        /// @brief 消息
        std::string message;
        /// @brief 函数名称
        std::string function_name;
        /// @brief 行号
        int64_t line_number;
        /// @brief 文件名称
        std::string file_name;
        /// @brief 发生时间
        std::chrono::system_clock::time_point occurrence_time;

        /**
         * @brief 转换为字符串
         * @return 事件字符串
         */
        std::string to_string()const;
    };

    /**
     * @brief 从字符串转换为诊断事件等级
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 DiagnosticEventLevel::Unknown
     */
    template <>
    DiagnosticEventLevel enum_cast<DiagnosticEventLevel>(const std::string& enum_string);
}

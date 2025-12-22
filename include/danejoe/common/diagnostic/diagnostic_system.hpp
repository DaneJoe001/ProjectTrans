/**
 * @file diagnostic_system.hpp
 * @brief 诊断系统
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-19
 */
#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <functional>
#include <thread>

#include "danejoe/common/diagnostic/diagnostic_event.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 格式化消息
     * @param message 消息
     * @return 格式化后的消息
     */
    std::string format_message(std::string_view message);
    /**
     * @brief 格式化消息
     * @tparam Args 参数类型
     * @param message_fmt 消息格式字符串
     * @param args 格式化参数
     * @return 格式化后的消息
     */
    template <typename... Args>
    std::string format_message(std::format_string<Args...> message_fmt, Args... args)
    {
        return std::format(message_fmt, std::forward<Args>(args)...);
    }
    /**
     * @class DiagnosticSystem
     * @brief 诊断系统
     */
    class DiagnosticSystem
    {
    public:
        /**
         * @brief 获取单例实例
         * @return 单例实例
         */
        static DiagnosticSystem& get_instance();
        /**
         * @brief 添加诊断事件
         * @param level 事件等级
         * @param module_name 模块名称
         * @param message 事件消息
         * @param line_number 行号
         * @param function_name 函数名称
         * @param file_name 文件名称
         */
        void add_event(DiagnosticEventLevel level,
            const std::string& module_name,
            const std::string& message, int64_t line_number,
            const std::string& function_name,
            const std::string& file_name);
        /**
         * @brief 添加诊断事件
         * @param event 诊断事件
         */
        void add_event(const DiagnosticEvent& event);
        /**
         * @brief 清空事件
         */
        void clear_events();
        /**
         * @brief 获取事件列表
         * @return 事件列表
         */
        std::vector<DiagnosticEvent> get_events();
        /**
         * @brief 按模块获取事件列表
         * @param module_name 模块名称
         * @return 事件列表
         */
        std::vector<DiagnosticEvent>
            get_events_by_module(const std::string& module_name);
        // std::optional<DiagnosticEvent> get_last_event();
    private:
        DiagnosticSystem();
        ~DiagnosticSystem();
    private:
        /// @brief 事件标识符计数器
        std::atomic<int64_t> m_event_id = 0;
        /// @brief 互斥锁
        std::mutex m_mutex;
        /// @brief 事件列表
        std::vector<DiagnosticEvent> m_events;
        /// @brief 线程 id 哈希器
        std::hash<std::thread::id> m_thread_id_hash;
    };
} // namespace DaneJoe

/**
 * @brief 添加诊断事件宏
 * @param level 事件等级
 * @param module 模块名称
 * @param ... 消息格式化参数：支持传入 `std::string_view message` 或 `std::format_string<Args...> message_fmt, Args... args`
 */
#define ADD_DIAGNOSTIC_EVENT(level, module, ...)                         \
  do {                                                                   \
    auto &diagnostic_system = DaneJoe::DiagnosticSystem::get_instance(); \
    diagnostic_system.add_event(                                         \
        level,                                                           \
        std::string(module),                                             \
        DaneJoe::format_message(__VA_ARGS__),                            \
        __LINE__,                                                        \
        std::string(__FUNCTION__),                                       \
        std::string(__FILE__));                                          \
  } while (0)

#define ADD_DIAG_TRACE(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Trace, module, __VA_ARGS__)
#define ADD_DIAG_DEBUG(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Debug, module, __VA_ARGS__)
#define ADD_DIAG_INFO(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Info, module, __VA_ARGS__)
#define ADD_DIAG_WARN(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Warn, module, __VA_ARGS__)
#define ADD_DIAG_ERROR(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Error, module, __VA_ARGS__)
#define ADD_DIAG_FATAL(module, ...) \
  ADD_DIAGNOSTIC_EVENT(DaneJoe::DiagnosticEventLevel::Fatal, module, __VA_ARGS__)

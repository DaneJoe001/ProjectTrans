/**
 * @file i_logger.hpp
 * @brief 日志接口类头文件
 * @details 定义日志抽象接口 ILogger 及日志创建器接口 ILoggerCreator。
 *          ILogger 提供 trace/debug/info/warn/error/fatal 等便捷模板接口，用于格式化日志内容并委托给 log_msg()。
 *          具体日志输出方式（同步/异步、文件/控制台等）由派生类实现。
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <format>
#include <string>
#include <thread>
#include <memory>

#include "danejoe/logger/logger_config.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ILogger
     * @brief 日志接口类
     * @details 提供统一日志接口与通用格式化能力。
     *          trace/debug/info/warn/error/fatal 负责格式化消息与收集元信息（文件/函数/行号、进程/线程 ID），
     *          并最终调用 log_msg() 交由具体实现输出。
     * @note 线程安全性由具体实现决定；通常期望允许多线程并发调用日志接口。
     */
    class ILogger
    {
    public:
        /**
         * @brief 构造函数
         */
        ILogger();
        /**
         * @brief 构造函数
         * @param config 日志配置
         */
        ILogger(LoggerConfig config);
        /**
         * @brief 跟踪级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
         */
        template<typename... Args>
        void trace(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::TRACE,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 调试级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
         */
        template<typename... Args>
        void debug(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::DEBUG,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 信息级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
         */
        template<typename... Args>
        void info(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::INFO,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 警告级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
          */
        template<typename... Args>
        void warn(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::WARN,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 错误级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
         */
        template<typename... Args>
        void error(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::ERROR,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 致命错误级别日志
         * @tparam Args 可变参数类型
         * @param module 模块名称
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param fmt 格式化字符串
         * @param args 可变参数
         */
        template<typename... Args>
        void fatal(const std::string& module,
            const std::string& file_name,
            const std::string& function_name,
            int line_num,
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            std::string log_info = std::format(fmt, std::forward<Args>(args)...);
            log_msg(LogLevel::FATAL,
                module,
                log_info,
                file_name,
                function_name,
                line_num,
                get_pid(),
                to_string(std::this_thread::get_id()));
        }
        /**
         * @brief 析构函数
         */
        virtual ~ILogger() = default;
        /**
         * @brief 将日志级别转换为字符串
         * @param level 日志级别
         * @return 日志级别字符串
         */
        std::string to_string(LogLevel level);
        /**
         * @brief 将字符串转换为日志级别
         * @param level_str 日志级别字符串
         * @return 日志级别
         */
        LogLevel to_log_level(std::string level_str);
        /**
         * @brief 设置日志输出设置
         * @param settings 日志输出设置
         * @details 设置输出目标与输出行为（具体含义见 LogOutputSetting 定义）。
         */
        void set_output_settings(const LogOutputSetting& settings);
        /**
         * @brief 设置日志配置
         * @param config 日志配置
         * @details 设置日志器的全局配置（例如等级、输出路径、格式等，取决于 LoggerConfig）。
         */
        void set_config(const LoggerConfig& config);
        /**
         * @brief 获取进程ID
         * @return 进程ID
         */
        static int get_pid();
        /**
         * @brief 将线程ID转换为字符串
         * @param thread_id 线程ID
         * @return 线程ID字符串
         */
        std::string to_string(const std::thread::id& thread_id);
        /**
         * @brief 获取日志头
         * @param level 日志级别
         * @param module 模块名称
         * @param log_info 日志信息
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param process_id 进程ID
         * @param thread_id 线程ID
         * @return 日志头
         * @details 根据日志级别与上下文信息构造一段可读的日志头部字符串。
         */
        std::string get_header(
            LogLevel level,
            const std::string& module,
            const std::string& log_info,
            const std::string& file_name = "",
            const std::string& function_name = "",
            int line_num = -1,
            int process_id = -1,
            const std::string& thread_id = "");
    protected:
        /**
         * @brief 日志消息
         * @param level 日志级别
         * @param module 模块名称
         * @param log_info 日志信息
         * @param file_name 文件名称
         * @param function_name 函数名称
         * @param line_num 行号
         * @param process_id 进程ID
         * @param thread_id 线程ID
         * @details 由派生类实现具体输出逻辑（例如同步写文件、异步队列写文件、控制台输出等）。
         */
        virtual void log_msg(
            LogLevel level,
            const std::string& module,
            const std::string& log_info,
            const std::string& file_name = "",
            const std::string& function_name = "",
            int line_num = -1,
            int process_id = -1,
            const std::string& thread_id = "") = 0;
    protected:
        /// @brief 日志配置
        LoggerConfig m_config = LoggerConfig();
        /// @brief 日志输出设置
        LogOutputSetting m_output_setting = LogOutputSetting();

    };
    /**
     * @class ILoggerCreator
     * @brief 日志创建器接口类
     * @details 用于基于配置创建具体 ILogger 实例。
     */
    class ILoggerCreator
    {
    public:
        /**
         * @brief 创建日志器
         * @param config 日志配置
         * @return 日志器
         */
        virtual std::shared_ptr<ILogger> operator()(const LoggerConfig& config) = 0;
    };
};

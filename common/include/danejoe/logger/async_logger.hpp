/**
 * @file async_logger.hpp
 * @brief DaneJoe异步日志类头文件
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 AsyncLogger，一个基于后台线程的异步日志器实现。
 *          log_msg() 将日志写入内部队列，并由后台线程负责输出到文件/控制台。
 */
#pragma once

#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "danejoe/logger/i_logger.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class AsyncLogger
     * @brief 异步日志器
     * @details 通过队列与后台线程实现异步写日志。
     *          - 文件日志与控制台日志分别由独立线程处理
     *          - stop_async_log() 用于通知后台线程退出，并应唤醒等待中的线程
     * @note 线程安全性取决于实现对队列与输出资源的同步策略；通常允许多线程并发调用 log_msg()。
     */
    class AsyncLogger : public ILogger
    {
    public:
        /**
         * @brief 构造函数
         * @details 创建日志器并初始化异步线程（具体行为由实现决定）。
         */
        AsyncLogger();
        /**
         * @brief 析构函数
         * @details 析构时应停止后台线程并回收资源。
         */
        ~AsyncLogger();
        /**
         * @brief 构造函数
         * @param config 日志配置
         * @details 使用指定配置初始化日志器。
         */
        AsyncLogger(const LoggerConfig& config);
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
         * @details 将日志格式化后写入内部队列，由后台线程异步输出。
         */
        virtual void log_msg(LogLevel level,
            const std::string& module,
            const std::string& log_info,
            const std::string& file_name = "",
            const std::string& function_name = "",
            int line_num = -1,
            int process_id = -1,
            const std::string& thread_id = "")override;
    private:
        /**
         * @brief 启动异步日志
         * @details 启动后台线程与相关资源。
         */
        void start_async_log();
        /**
         * @brief 停止异步日志
         * @details 通知后台线程退出，并唤醒等待中的线程。
         */
        void stop_async_log();
        /**
         * @brief 打开日志文件
         * @return 是否成功
         * @details 根据配置打开/创建日志文件。
         */
        bool open_log_file();
        /**
         * @brief 异步文件日志处理
         * @details 后台线程循环从文件日志队列中取出消息并写入文件。
         */
        void async_file_log_handler();
        /**
         * @brief 异步控制台日志处理
         * @details 后台线程循环从控制台日志队列中取出消息并输出到控制台。
         */
        void async_console_log_handler();
    private:
        /// @brief 异步文件日志标志
        std::atomic<bool> m_async_file_log_flag = false;
        /// @brief 异步控制台日志标志
        std::atomic<bool> m_async_console_log_flag = false;
        /// @brief 控制台互斥锁
        std::mutex m_console_mutex;
        /// @brief 文件互斥锁
        std::mutex m_file_mutex;
        /// @brief 日志文件
        std::ofstream m_log_file;
        /// @brief 调用栈
        std::queue<std::string> m_call_stack;
        /// @brief 文件日志队列互斥锁
        std::mutex m_file_log_queue_mutex;
        /// @brief 控制台日志队列互斥锁
        std::mutex m_console_log_queue_mutex;
        /// @brief 文件日志队列
        std::queue<std::string> m_file_log_queue;
        /// @brief 控制台日志队列
        std::queue<std::string> m_console_log_queue;
        /// @brief 异步文件日志线程
        std::thread m_async_file_log_thread;
        /// @brief 异步控制台日志线程
        std::thread m_async_console_log_thread;
        /// @brief 文件日志队列条件变量
        std::condition_variable m_file_log_queue_cond;
        /// @brief 控制台日志队列条件变量
        std::condition_variable m_console_log_queue_cond;

    };
    /**
     * @class DaneJoeLoggerCreator
     * @brief DaneJoe日志创建器类
     */
    class DaneJoeLoggerCreator : public ILoggerCreator
    {
    public:
        /**
         * @brief 创建日志器
         * @param config 日志配置
         * @return 日志器
         */
        std::shared_ptr<ILogger> operator()(const LoggerConfig& config) override;
    };
}

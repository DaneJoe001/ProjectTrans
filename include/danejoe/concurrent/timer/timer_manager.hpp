/**
 * @file timer_manager.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 定时器管理器
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <map>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <condition_variable>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @struct PeriodicTask
     * @brief 周期任务
     */
    struct PeriodicTask
    {
        /// @brief 间隔周期
        std::chrono::steady_clock::duration time;
        /// @brief 周期回调函数
        std::function<void()> callback;
        /// @brief 剩余执行次数,-1表示无限次
        int64_t remain_times;
    };
    /**
     * @class TimerManager
     * @brief 定时器管理器
     */
    class TimerManager
    {
    public:
        /**
         * @brief 获取单例实例
         * @return 单例实例
         */
        static TimerManager& get_instance();
        /**
         * @brief 获取系统时间
         * @return 系统时间 std::chrono::system_clock::time_point
         */
        std::chrono::system_clock::time_point get_system_time()const;
        /**
         * @brief 获取稳定时间
         * @return 稳定时间 std::chrono::steady_clock::time_point
         */
        std::chrono::steady_clock::time_point get_steady_time()const;
        /**
         * @brief 添加任务直到指定时间
         * @param time 指定时间
         * @param callback 回调函数
         */
        void add_task_until(const std::chrono::steady_clock::time_point& time, std::function<void()> callback);
        /**
         * @brief 添加任务直到指定时长
         * @param time 指定时长
         * @param callback 回调函数
         */
        void add_task_for(const std::chrono::steady_clock::duration& time, std::function<void()> callback);
        /**
         * @brief 添加周期任务
         * @param time 周期时长
         * @param callback 回调函数
         * @param remain_times 剩余执行次数,-1表示无限次
         * @return 任务ID
         */
        uint64_t add_periodic_task(const std::chrono::steady_clock::duration& time, std::function<void()> callback, int64_t remain_times = -1);
        /**
         * @brief 取消周期任务
         * @param task_id 任务ID
         */
        void cancel_periodic_task(uint64_t task_id);
        /**
         * @brief 设置执行环境
         * @param execute_environment 执行环境
         */
        void set_execute_environment(std::function<void(std::function<void()>)> execute_environment);
    private:
        /**
         * @brief 执行周期任务
         * @param task_id 任务ID
         */
        void execute_periodic_task(uint64_t task_id);
        /**
         * @brief 构造函数
         */
        TimerManager();
        /**
         * @brief 拷贝构造函数
         * @param other 其他定时器管理器
         */
        TimerManager(const TimerManager&) = delete;
        /**
         * @brief 移动构造函数
         * @param other 其他定时器管理器
         */
        TimerManager(const TimerManager&&) = delete;
        /**
         * @brief 拷贝赋值运算符
         * @param other 其他定时器管理器
         */
        TimerManager& operator=(const TimerManager&) = delete;
        /**
         * @brief 移动赋值运算符
         * @param other 其他定时器管理器
         */
        TimerManager& operator=(const TimerManager&&) = delete;
        /**
         * @brief 析构函数
         */
        ~TimerManager();
    private:
        /// @brief 用于周期任务ID生成
        std::atomic<uint64_t> m_cycle_task_id = 0;
        /// @brief 周期任务回调列表
        std::unordered_map<uint64_t, PeriodicTask> m_periodic_tasks;
        /// @brief 周期任务锁
        std::mutex m_periodic_task_mutex;
        /// @brief 是否运行
        std::atomic<bool> m_is_running = false;
        /// @brief 任务队列锁，用于处理任务队列
        std::mutex m_task_queue_mutex;
        /// @brief 任务队列条件变量，用于处理任务队列
        std::condition_variable m_task_queue_condition;
        /// @brief 定时器回调队列，用于处理定时任务
        std::multimap<std::chrono::steady_clock::time_point, std::function<void()>> m_timer_callbacks;
        /// @brief 任务线程，用于处理任务
        std::thread m_task_thread;
        /// @brief 任务队列，用于处理任务
        std::deque<std::function<void()>> m_task_queue;
        /// @brief 执行环境，用于处理任务
        std::function<void(std::function<void()>)> m_execute_environment;
    };
}


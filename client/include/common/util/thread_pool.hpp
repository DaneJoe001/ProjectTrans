/**
 * @file thread_pool.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 线程池
 * @date 2025-12-28
 */
#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <cstdint>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

 /**
  * @class ThreadPool
  * @brief 线程池
  */
class ThreadPool
{
public:
    /**
     * @brief 构造函数
     * @param max_thread_count 最大线程数
     * @param max_task_count 最大任务数
     */
    ThreadPool(int32_t max_thread_count, int32_t max_task_count);
    /**
     * @brief 启动线程池
     */
    void start();
    /**
     * @brief 停止线程池
     */
    void stop();
    /**
     * @brief 添加任务
     * @param task 任务
     */
    void add_task(std::function<void()> task);
private:
    /**
     * @brief 线程处理函数
     */
    void thread_handler();
private:
    /// @brief 最大线程数
    int32_t m_max_thread_count = 0;
    /// @brief 最大任务数
    int32_t m_max_task_count = 0;
    /// @brief 是否运行
    std::atomic<bool> m_is_running = false;
    /// @brief 任务队列
    DaneJoe::MpmcBoundedQueue<std::function<void()>> m_task_queue;
    /// @brief 线程
    std::vector<std::thread> m_threads;
};
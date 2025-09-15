#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <cstdint>

#include "mt_queue/mt_queue.hpp"

class ThreadPool
{
public:
    ThreadPool(int32_t max_thread_count, int32_t max_task_count);
    void start();
    void add_task(std::function<void()> task);
    void stop();
private:
    void thread_handler();
private:
    int32_t m_max_thread_count = 0;
    int32_t m_max_task_count = 0;
    std::atomic<bool> m_is_running = false;
    DaneJoe::MTQueue<std::function<void()>> m_task_queue;
    std::vector<std::thread> m_threads;
};
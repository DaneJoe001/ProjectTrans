#pragma once

#include <thread>
#include <vector>
#include <functional>

#include "mt_queue/mt_queue.hpp"

class ThreadPool
{
public:
    ThreadPool(int max_thread_count, int max_task_count);
    void start();
    void add_task(std::function<void()> task);
    void stop();
private:
    void thread_handler();
private:
    int m_max_thread_count = 0;
    int m_max_task_count = 0;
    std::atomic<bool> m_is_running = false;
    DaneJoe::MTQueue<std::function<void()>> m_task_queue;
    std::vector<std::thread> m_threads;
};
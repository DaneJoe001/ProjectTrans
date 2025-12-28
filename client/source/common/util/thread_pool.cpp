#include "common/util/thread_pool.hpp"

void ThreadPool::start()
{
    for (int32_t i = 0;i < m_max_thread_count;i++)
    {
        // 入队并启动线程函数
        // m_threads.emplace_back(thread_handler);
    }
}

void ThreadPool::add_task(std::function<void()> task)
{
    // 添加任务
    m_task_queue.push(task);
}

void ThreadPool::stop()
{
    // 停止多线程队列
    m_task_queue.close();
}

void ThreadPool::thread_handler()
{
    while (m_is_running.load())
    {

    }
}
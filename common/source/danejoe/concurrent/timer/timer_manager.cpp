#include "danejoe/concurrent/timer/timer_manager.hpp"

using namespace std::chrono_literals;

DaneJoe::TimerManager& DaneJoe::TimerManager::get_instance()
{
    static TimerManager instance;
    return instance;
}

std::chrono::system_clock::time_point DaneJoe::TimerManager::get_system_time()const
{
    return std::chrono::system_clock::now();
}

std::chrono::steady_clock::time_point DaneJoe::TimerManager::get_steady_time()const
{
    return std::chrono::steady_clock::now();
}

void DaneJoe::TimerManager::add_task_until(
    const std::chrono::steady_clock::time_point& time, std::function<void()> callback)
{
    {
        std::lock_guard<std::mutex> lock(m_task_queue_mutex);
        m_timer_callbacks.insert(std::make_pair(time, callback));
    }
    m_task_queue_condition.notify_one();
}
void DaneJoe::TimerManager::add_task_for(const std::chrono::steady_clock::duration& time, std::function<void()> callback)
{
    std::chrono::steady_clock::time_point deadline = get_steady_time() + time;
    {
        std::lock_guard<std::mutex> lock(m_task_queue_mutex);
        m_timer_callbacks.insert(std::make_pair(deadline, callback));
    }
    m_task_queue_condition.notify_one();
}

uint64_t DaneJoe::TimerManager::add_periodic_task(const std::chrono::steady_clock::duration& time, std::function<void()> callback, int64_t remain_times)
{
    uint64_t task_id = m_cycle_task_id.fetch_add(1);
    PeriodicTask task;
    task.callback = std::move(callback);
    task.remain_times = remain_times;
    task.time = time;
    {
        std::lock_guard<std::mutex> lock(m_periodic_task_mutex);
        m_periodic_tasks.emplace(task_id, task);
    }
    add_task_for(time, [task_id, this]()
        {
            execute_periodic_task(task_id);
        });
    return task_id;
}

void DaneJoe::TimerManager::execute_periodic_task(uint64_t task_id)
{

    std::function<void()> callback;
    bool is_execute = false;
    bool is_can_cancel = false;
    {
        std::lock_guard<std::mutex> lock(m_periodic_task_mutex);
        auto task = m_periodic_tasks.find(task_id);
        if (task == m_periodic_tasks.end())
        {
            return;
        }
        auto time = task->second.time;
        callback = task->second.callback;
        auto times = task->second.remain_times;
        if (times > 0)
        {
            task->second.remain_times--;
            is_execute = true;
        }
        else if (times < 0)
        {
            is_execute = true;
        }
        if (times != 0)
        {
            // 添加下一轮任务回调
            add_task_for(time, [task_id, this]()
                {
                    execute_periodic_task(task_id);
                });
        }
        else
        {
            is_can_cancel = true;
        }
    }
    if (is_execute)
    {
        callback();
    }
    if (is_can_cancel)
    {
        cancel_periodic_task(task_id);
    }
}
void DaneJoe::TimerManager::cancel_periodic_task(uint64_t task_id)
{
    std::lock_guard<std::mutex> lock(m_periodic_task_mutex);
    m_periodic_tasks.erase(task_id);
}

void DaneJoe::TimerManager::set_execute_environment(std::function<void(std::function<void()>)> execute_environment)
{
    m_execute_environment = execute_environment;
}

DaneJoe::TimerManager::TimerManager()
{
    m_is_running.store(true);


    auto timer_thread_lambda = [this]()
        {
            while (m_is_running.load())
            {
                {
                    std::unique_lock<std::mutex> lock(m_task_queue_mutex);
                    auto wait_time = m_timer_callbacks.empty() ? std::chrono::steady_clock::time_point::max() : m_timer_callbacks.begin()->first;
                    m_task_queue_condition.wait_until(
                        lock,
                        wait_time,
                        [this, wait_time]()
                        {
                            if (!m_is_running.load())
                            {
                                return true;
                            }
                            if (m_timer_callbacks.empty())
                            {
                                return false;
                            }
                            return m_timer_callbacks.begin()->first < wait_time;
                        });
                    if (!m_is_running.load())
                    {
                        return;
                    }
                    wait_time = m_timer_callbacks.empty() ? std::chrono::steady_clock::time_point::max() : m_timer_callbacks.begin()->first;
                    auto now = get_steady_time();
                    if (wait_time > now ||
                        m_timer_callbacks.empty())
                    {
                        continue;
                    }
                    while (!m_timer_callbacks.empty() &&
                        m_timer_callbacks.begin()->first <= now)
                    {
                        auto it = m_timer_callbacks.begin();
                        m_task_queue.push_back(std::move(it->second));
                        m_timer_callbacks.erase(it);
                    }
                }
                while (!m_task_queue.empty())
                {
                    if (m_execute_environment)
                    {
                        m_execute_environment(m_task_queue.front());
                    }
                    else
                    {
                        m_task_queue.front()();
                    }
                    m_task_queue.pop_front();
                }
            }
        };
    m_task_thread = std::thread(timer_thread_lambda);
}
DaneJoe::TimerManager::~TimerManager()
{
    m_is_running.store(false);
    m_task_queue_condition.notify_one();
    if (m_task_thread.joinable())
    {
        m_task_thread.join();
    }
}

/**
 * @file mpmc_bounded_queue.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 线程安全的队列
 * @version 0.2.0
 * @date 2025-12-17
 * @details 提供多生产者多消费者（MPMC）的有界阻塞队列实现。
 *          支持阻塞/非阻塞弹出与插入，以及 close() 关闭语义（唤醒所有等待线程）。
 */
#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include <optional>
#include <iterator>
#include <condition_variable>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class MpmcBoundedQueue
     * @brief 线程安全的有界队列
     * @tparam T 队列元素类型
     * @details 多生产者多消费者（MPMC）有界队列：
     *          - push/pop 在队列满/空时会阻塞等待
     *          - try_pop 为非阻塞版本
     *          - close() 会将队列置为非运行状态并唤醒等待线程
     * @note close() 后不再接受新元素；已存在的元素仍可被消费。
     */
    template<class T>
    class MpmcBoundedQueue
    {
    public:
        /**
         * @brief 构造函数
         * @param max_size 队列最大容量
         */
        MpmcBoundedQueue(int max_size = 50) : m_max_size(max_size) {}
        /**
         * @brief 弹出队首元素
         * @return 弹出的元素；当队列关闭且为空时返回 std::nullopt
         * @details 当队列为空时会阻塞等待；close() 后若队列已空，则返回 std::nullopt。
         */
        std::optional<T> pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_empty_cv.wait(lock, [this]()
                {
                    return !m_queue.empty() || !m_is_running;
                });
            if (m_queue.empty())
            {
                return std::nullopt;
            }
            T item = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();
            m_full_cv.notify_one();
            return item;
        }
        /**
         * @brief 批量弹出队首元素
         * @param nums 要弹出的元素数量
         * @return 批量弹出的元素；当队列关闭且无法满足 nums 时返回 std::nullopt
         * @details 当队列为空时会阻塞等待。
         *          若 close() 且队列已空、无法继续弹出以满足 nums，则返回 std::nullopt。
         */
        std::optional<std::vector<T>> pop(int nums)
        {

            int has_popped = 0;
            std::vector<T> result;
            std::unique_lock<std::mutex> lock(m_mutex);
            while (has_popped < nums)
            {
                m_empty_cv.wait(lock, [this]()
                    {
                        return !m_queue.empty() || !m_is_running;
                    });
                if (m_queue.empty() && !m_is_running)
                {
                    return std::nullopt;
                }
                result.emplace_back(std::move(m_queue.front()));
                has_popped++;
                m_queue.pop();
            }
            lock.unlock();
            m_full_cv.notify_one();
            return result;
        }
        /**
         * @brief 尝试弹出队首元素
         * @return 弹出的元素；队列为空时返回 std::nullopt
         * @details 非阻塞：若当前队列为空则立即返回 std::nullopt。
         */
        std::optional<T> try_pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_queue.empty())
            {
                return std::nullopt;
            }
            T item = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();
            m_full_cv.notify_one();
            return item;
        }
        /**
         * @brief 尝试打包弹出元素
         * @param nums 要弹出的元素数量
         * @return std::vector<T> 批量弹出的元素
         * @details 非阻塞：最多弹出 nums 个元素；若队列为空则返回当前已收集结果（可能为空）。
         */
        std::vector<T> try_pop(std::size_t nums)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            int has_popped = 0;
            std::vector<T> result;
            while (has_popped < nums)
            {
                if (m_queue.empty())
                {
                    return result;
                }
                result.push_back(std::move(m_queue.front()));
                m_queue.pop();
                has_popped++;
            }
            lock.unlock();
            m_full_cv.notify_all();
            return result;
        }
        /**
         * @brief 等待弹出队首元素
         * @tparam Period 等待时间类型
         * @param timeout 等待时间
         * @return 弹出的元素；超时/队列关闭且为空时返回 std::nullopt
         * @details 在 timeout 之前等待队列非空或队列被关闭。
         */
        template<class Period>
        std::optional<T> pop_until(Period timeout)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_empty_cv.wait_until(lock, timeout, [this]()
                {
                    return !m_queue.empty() || !m_is_running;
                });
            if (m_queue.empty())
            {
                return std::nullopt;
            }
            T item = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();
            m_full_cv.notify_one();
            return item;
        }
        /**
         * @brief 等待弹出队首元素
         * @tparam Period 等待时间类型
         * @param timeout 等待时间
         * @return 弹出的元素；超时/队列关闭且为空时返回 std::nullopt
         * @details 在 timeout 时长内等待队列非空或队列被关闭。
         */
        template<class Period>
        std::optional<T> pop_for(Period timeout)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_empty_cv.wait_for(lock, timeout, [this]()
                {
                    return !m_queue.empty() || !m_is_running;
                });
            if (m_queue.empty())
            {
                return std::nullopt;
            }
            T item = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();
            m_full_cv.notify_one();
            return item;
        }
        /**
         * @brief 判断队列是否为空
         * @return true 队列为空
         * @return false 队列不为空
         */
        bool is_empty()const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_queue.empty();
        }
        /**
         * @brief 判断队列是否已满
         * @return true 队列已满
         * @return false 队列未满
         */
        bool is_full()const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_queue.size() >= m_max_size;
        }
        /**
         * @brief 获取队列大小
         * @return std::size_t 队列大小
         */
        std::size_t size()const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_queue.size();
        }
        /**
         * @brief 判断队列是否正在运行
         * @return true 队列正在运行
         * @return false 队列未运行
         */
        bool is_running()const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_is_running;
        }
        /**
         * @brief 获取队首元素
         * @note 模板元素需要可拷贝
         * @return 队首元素的拷贝；当队列关闭且为空时返回 std::nullopt
         * @details 当队列为空时会阻塞等待；close() 后若队列已空，则返回 std::nullopt。
         */
        std::optional<T> front()const
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_empty_cv.wait(lock, [this]()
                {
                    return !m_queue.empty() || !m_is_running;
                });
            if (!m_queue.empty())
            {
                T item = m_queue.front();
                return item;
            }
            return std::nullopt;
        }
        /**
         * @brief 添加元素到队列
         * @param item 元素
         * @return true 添加成功
         * @return false 队列已关闭
         * @details 当队列已满时会阻塞等待；若在等待期间 close()，则返回 false。
         */
        bool push(T item)
        {
            bool is_pushed = false;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_is_running)
                {
                    m_full_cv.wait(lock, [this]()
                        {
                            return !m_is_running || (m_queue.size() < m_max_size);
                        });
                    if (!m_is_running)
                    {
                        return false;
                    }
                    m_queue.push(std::move(item));
                    is_pushed = true;
                }
            }
            if (is_pushed)
            {
                m_empty_cv.notify_one();
            }
            return is_pushed;
        }
        /**
         * @brief 添加元素到队列
         * @tparam U 元素类型
         * @param begin 元素起始迭代器
         * @param end 元素结束迭代器
         * @return true 添加成功
         * @return false 队列已关闭
         * @details 当队列空间不足时会阻塞等待；若在等待期间 close()，则返回 false。
         */
        template<typename U>
        bool push(U begin, U end)
        {
            std::size_t nums = static_cast<int>(std::distance(begin, end));
            bool is_pushed = false;

            while (nums > 0)
            {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    if (!m_is_running)
                    {
                        return false;
                    }
                    m_full_cv.wait(lock, [this]()
                        {
                            return (m_queue.size() < m_max_size) || !m_is_running;
                        });
                    if (!m_is_running)
                    {
                        return false;
                    }
                    std::size_t to_insert = std::min(nums, m_max_size - m_queue.size());
                    for (std::size_t i = 0; i < to_insert; ++i)
                    {
                        m_queue.push(*begin);
                        ++begin;
                    }
                    nums -= to_insert;
                    is_pushed = true;
                }
                if (is_pushed)
                {
                    m_empty_cv.notify_all();
                }
            }
            return is_pushed;
        }
        /**
         * @brief 析构函数
         */
        ~MpmcBoundedQueue()noexcept
        {
            close();
        }
        /**
         * @brief 移动构造函数
         * @param other 其他队列
         */
        MpmcBoundedQueue(MpmcBoundedQueue&& other) noexcept
        {
            std::scoped_lock<std::mutex, std::mutex> lock(m_mutex, other.m_mutex);
            m_queue = std::move(other.m_queue);
            m_is_running = other.m_is_running;
            other.m_is_running = false;
        }
        /**
         * @brief 移动赋值运算符
         * @param other 其他队列
         * @return MpmcBoundedQueue&
         */
        MpmcBoundedQueue& operator=(MpmcBoundedQueue&& other)noexcept
        {
            if (this == &other)
            {
                return *this;
            }
            std::scoped_lock<std::mutex, std::mutex> lock(m_mutex, other.m_mutex);
            m_queue = std::move(other.m_queue);
            m_is_running = other.m_is_running;
            other.m_is_running = false;
            return *this;
        }
        /**
         * @brief 关闭队列
         * @note 关闭后，队列不再接受新元素，但已有的元素仍然可以被弹出
         * @details 将队列置为非运行状态，并唤醒所有等待在空/满条件变量上的线程。
         */
        void close()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_is_running = false;
            }
            m_empty_cv.notify_all();
            m_full_cv.notify_all();
        }
        /**
         * @brief 设置队列最大长度
         * @param max_size 最大长度
         */
        void set_max_size(std::size_t max_size)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::size_t temp_raw = m_max_size;
            m_max_size = max_size;
            if (temp_raw < max_size)
            {
                m_full_cv.notify_all();
            }
        }
        /**
         * @brief 获取队列最大长度
         * @return std::size_t
         */
        std::size_t get_max_size() const
        {
            return m_max_size;
        }
    private:
        /**
         * @brief 拷贝构造函数
         * @note 禁止拷贝构造
         */
        MpmcBoundedQueue(const MpmcBoundedQueue&) = delete;
        /**
         * @brief 拷贝赋值运算符
         * @note 禁止拷贝赋值
         */
        MpmcBoundedQueue& operator=(const MpmcBoundedQueue&) = delete;
    private:
        /// @brief 队列最大容量
        std::size_t m_max_size = 0;
        /// @brief 互斥锁
        mutable std::mutex m_mutex;
        /// @brief 空条件变量
        mutable std::condition_variable m_empty_cv;
        /// @brief 满条件变量
        mutable std::condition_variable m_full_cv;
        /// @brief 队列
        std::queue<T> m_queue;
        /// @brief 是否正在运行
        bool m_is_running = true;
    };

}

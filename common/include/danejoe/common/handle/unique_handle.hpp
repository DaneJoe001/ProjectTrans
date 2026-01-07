/**
 * @file unique_handle.hpp
 * @brief RAII 句柄封装
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供对 POSIX 句柄（如文件描述符）的 RAII 封装：构造时接管句柄，析构时自动 close。
 *          该类型仅支持移动，不支持拷贝；release() 可释放所有权。
 */
#pragma once

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/common/status/status_code.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 关闭句柄
     * @param handle 句柄引用
     * @return 状态码
     * @details 该函数用于对句柄执行关闭操作。
     */
    StatusCode close(int& handle);

    /**
     * @class UniqueHandle
     * @brief 句柄 RAII 封装
     * @tparam T 句柄类型（需满足 is_posix_handle<T>::value）
     * @details 以独占所有权方式管理句柄：
     *          - 析构时自动 close
     *          - 支持移动构造/移动赋值
     *          - release() 释放所有权且不再关闭
     * @note 约定句柄值 < 0 表示无效。
     */
    template<class T, std::enable_if_t<
        is_posix_handle<T>::value, int> = 0>
    class UniqueHandle
    {
    public:
        /**
         * @brief 默认构造（无效句柄）
         */
        UniqueHandle() {}
        /**
         * @brief 构造并接管句柄
         * @param handle 需要接管的句柄
         */
        UniqueHandle(T handle) :m_handle(handle) {}
        /**
         * @brief 移动构造
         * @param other 源对象（被置为无效句柄）
         */
        UniqueHandle(UniqueHandle<T>&& other)noexcept
        {
            T new_handle = other.m_handle;
            other.m_handle = -1;
            m_handle = new_handle;
        }
        /**
         * @brief 析构函数
         * @details 若句柄有效则调用 close。
         */
        ~UniqueHandle()
        {
            close(m_handle);
            m_handle = -1;
        }
        /**
         * @brief 移动赋值
         * @param other 源对象（被置为无效句柄）
         * @return 当前对象引用
         */
        UniqueHandle<T>& operator=(UniqueHandle<T>&& other)noexcept
        {
            T new_handle = other.m_handle;
            other.m_handle = -1;
            if (m_handle >= 0)
            {
                close(m_handle);
            }
            m_handle = new_handle;
            return *this;
        }
        /**
         * @brief 是否持有有效句柄
         * @return true 句柄有效
         * @return false 句柄无效
         */
        explicit operator bool() const
        {
            return m_handle >= 0;
        }
        /**
         * @brief 判断句柄是否有效
         * @return true 句柄有效
         * @return false 句柄无效
         */
        bool is_valid()const
        {
            return m_handle >= 0;
        }
        /**
         * @brief 获取当前句柄
         * @return 句柄值
         */
        T get()const
        {
            return m_handle;
        }
        /**
         * @brief 释放句柄所有权
         * @return 被释放的句柄
         * @details 调用后对象变为无效句柄，析构时不再关闭该句柄。
         */
        T release()
        {
            T handle = m_handle;
            m_handle = -1;
            return handle;
        }
        /**
         * @brief 重置为新句柄
         * @param new_handle 新句柄
         * @details 会先对当前句柄执行 close，然后接管 new_handle。
         */
        void reset(T new_handle)
        {
            close(m_handle);
            m_handle = new_handle;
        }
    private:
        /**
         * @brief 禁止拷贝构造
         * @details 句柄为独占资源，禁止拷贝以避免重复关闭。
         */
        UniqueHandle(const UniqueHandle&) = delete;
        /**
         * @brief 禁止拷贝赋值
         * @details 句柄为独占资源，禁止拷贝以避免重复关闭。
         */
        UniqueHandle<T>& operator=(const UniqueHandle&) = delete;
    private:
        /// @brief 当前持有的句柄（<0 表示无效）
        T m_handle = -1;
    };
}

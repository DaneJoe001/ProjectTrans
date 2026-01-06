#pragma once

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/common/status/status_code.hpp"

namespace DaneJoe
{
    StatusCode close(int& handle);

    template<class T, std::enable_if_t<
        is_posix_handle<T>::value, int> = 0>
    class UniqueHandle
    {
    public:
        UniqueHandle() {}
        UniqueHandle(T handle) :m_handle(handle) {}
        UniqueHandle(UniqueHandle<T>&& other)noexcept
        {
            T new_handle = other.m_handle;
            other.m_handle = -1;
            m_handle = new_handle;
        }
        ~UniqueHandle()
        {
            close(m_handle);
            m_handle = -1;
        }
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
        explicit operator bool() const
        {
            return m_handle >= 0;
        }
        bool is_valid()const
        {
            return m_handle >= 0;
        }
        T get()const
        {
            return m_handle;
        }
        T release()
        {
            T handle = m_handle;
            m_handle = -1;
            return handle;
        }
        void reset(T new_handle)
        {
            close(m_handle);
            m_handle = new_handle;
        }
    private:
        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle<T>& operator=(const UniqueHandle&) = delete;
    private:
        T m_handle = -1;
    };
}

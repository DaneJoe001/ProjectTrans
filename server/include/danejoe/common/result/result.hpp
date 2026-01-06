#pragma once

#include <optional>
 #include <utility>

#include "danejoe/common/status/status_code.hpp"

namespace DaneJoe
{
    template<class T>
    class Result
    {
    public:
        Result(std::optional<T> value, const StatusCode& status_code) :
            m_value(std::move(value)),
            m_status_code(status_code)
        {}
        Result(const StatusCode& status_code) :
            m_value(std::nullopt),
            m_status_code(status_code)
        {}
        bool has_value()const
        {
            return m_value.has_value();
        }
        const T& value()const&
        {
            return m_value.value();
        }
        T& value()&
        {
            return m_value.value();
        }
        T&& value()&&
        {
            return std::move(m_value.value());
        }
        const StatusCode& status_code()const
        {
            return m_status_code;
        }
    private:
        std::optional<T> m_value = std::nullopt;
        StatusCode m_status_code;
    };
};
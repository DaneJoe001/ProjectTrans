#pragma once

#include <string>
#include <string_view>

namespace DaneJoe
{
    enum class StatusLevel
    {
        Ok,
        Branch,
        Error,
        Unknown
    };
    class IStatusDetail
    {
    public:
        virtual ~IStatusDetail() = 0;
        virtual std::string message()const = 0;
        virtual std::string_view domain()const = 0;
        virtual bool operator==(const IStatusDetail& rhs) const = 0;
        explicit operator bool()const;
        bool is_ok()const;
        bool is_error()const;
        bool is_branch()const;
        StatusLevel get_status_level()const;
    protected:
        StatusLevel m_status_level = StatusLevel::Unknown;
    };
}
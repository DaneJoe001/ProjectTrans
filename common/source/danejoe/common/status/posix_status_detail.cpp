#include <set>
#include <cerrno>
#include <cstring>

#include "danejoe/common/status/posix_status_detail.hpp"

DaneJoe::PosixStatusSet::PosixStatusSet()
{
    m_set = { EAGAIN,EWOULDBLOCK,EINPROGRESS };
}
DaneJoe::PosixStatusSet::PosixStatusSet(std::vector<int> branch_set)
{
    std::set<int> branch = std::set<int>(branch_set.begin(), branch_set.end());
    m_set = std::vector<int>(branch.begin(), branch.end());
}
bool DaneJoe::PosixStatusSet::has_status_code(int status_code)const
{
    for (const auto& value : m_set)
    {
        if (value == status_code)
        {
            return true;
        }
    }
    return false;
}

DaneJoe::PosixStatusDetail::PosixStatusDetail(
    std::optional<std::string> user_message,
    PosixStatusSet branch_set) :
    m_branch_set(branch_set),
    m_user_message(user_message)
{
    m_error_code = errno;
    m_status_level = to_status_level(m_error_code.value());
}
DaneJoe::PosixStatusDetail::PosixStatusDetail(
    bool is_ok,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set) :
    m_branch_set(branch_set),
    m_user_message(user_message)
{
    m_status_level = is_ok ? StatusLevel::Ok : StatusLevel::Error;
}
DaneJoe::PosixStatusDetail::PosixStatusDetail(
    int status_code,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set) :
    m_branch_set(branch_set),
    m_user_message(user_message)
{
    m_error_code = status_code;
    m_status_level = to_status_level(m_error_code.value());
}
DaneJoe::PosixStatusDetail::PosixStatusDetail(
    StatusLevel status_level,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set) :
    m_branch_set(branch_set),
    m_user_message(user_message)
{
    m_status_level = status_level;
}
std::string_view DaneJoe::PosixStatusDetail::domain()const
{
    return m_domain;
}
std::string DaneJoe::PosixStatusDetail::message()const
{
    if (!m_user_message.has_value() && !m_error_code.has_value())
    {
        return "No valid status message";
    }
    if (m_user_message.has_value())
    {
        return m_user_message.value();
    }
    return strerror(m_error_code.value());
}

bool DaneJoe::PosixStatusDetail::operator==(const IStatusDetail& rhs) const
{
    auto rhs_detail = dynamic_cast<const PosixStatusDetail*>(&rhs);
    if (!rhs_detail)
    {
        return false;
    }
    return rhs_detail->m_error_code == m_error_code;
}

DaneJoe::StatusLevel DaneJoe::PosixStatusDetail::to_status_level(int status_code)const
{
    if (status_code == 0)
    {
        return StatusLevel::Ok;
    }
    else if (m_branch_set.has_status_code(status_code))
    {
        return StatusLevel::Branch;
    }
    else
    {
        return StatusLevel::Error;
    }
}
std::shared_ptr<DaneJoe::PosixStatusDetail> DaneJoe::make_posix_status_detail(
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    return std::make_shared<PosixStatusDetail>(user_message, branch_set);
}
std::shared_ptr<DaneJoe::PosixStatusDetail> DaneJoe::make_posix_status_detail(
    bool is_ok,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    return std::make_shared<PosixStatusDetail>(is_ok, user_message, branch_set);
}
std::shared_ptr<DaneJoe::PosixStatusDetail> DaneJoe::make_posix_status_detail(
    int error_code,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    return std::make_shared<PosixStatusDetail>(error_code, user_message, branch_set);
}
std::shared_ptr<DaneJoe::PosixStatusDetail> DaneJoe::make_posix_status_detail(
    StatusLevel status_level,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    return std::make_shared<PosixStatusDetail>(status_level, user_message, branch_set);
}
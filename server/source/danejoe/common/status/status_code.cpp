#include "danejoe/common/status/status_code.hpp"

DaneJoe::StatusCode::StatusCode() {}

DaneJoe::StatusCode::StatusCode(std::shared_ptr<IStatusDetail> error_code) :m_status_detail(error_code) {}
std::string DaneJoe::StatusCode::message()const
{
    if (!m_status_detail)
    {
        return "Invalid Status";
    }
    return m_status_detail->message();
}
bool DaneJoe::StatusCode::is_error()const
{
    if (!m_status_detail)
    {
        return false;
    }
    return m_status_detail->is_error();
}
bool DaneJoe::StatusCode::is_ok()const
{
    if (!m_status_detail)
    {
        return false;
    }
    return m_status_detail->is_ok();
}
bool DaneJoe::StatusCode::is_branch()const
{
    if (!m_status_detail)
    {
        return false;
    }
    return m_status_detail->is_branch();
}
bool DaneJoe::StatusCode::is_valid()const
{
    return m_status_detail != nullptr;
}
DaneJoe::StatusLevel DaneJoe::StatusCode::get_status_level()const
{
    if (!m_status_detail)
    {
        return StatusLevel::Unknown;
    }
    return m_status_detail->get_status_level();
}
DaneJoe::StatusCode::operator bool()const
{
    if (!m_status_detail)
    {
        return false;
    }
    return m_status_detail->is_ok();
}

bool DaneJoe::StatusCode::operator==(const StatusCode& other)const
{
    if (!m_status_detail && !other.m_status_detail)
    {
        return true;
    }
    if (m_status_detail && other.m_status_detail)
    {
        return *m_status_detail == *other.m_status_detail;
    }
    return false;
}

bool DaneJoe::StatusCode::operator==(std::shared_ptr<IStatusDetail> other_detail)
{
    if (!m_status_detail && !other_detail)
    {
        return true;
    }
    if (m_status_detail && other_detail)
    {
        return *m_status_detail == *other_detail;
    }
    return false;
}
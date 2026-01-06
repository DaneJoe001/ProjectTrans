#include "danejoe/common/status/i_status_detail.hpp"

DaneJoe::IStatusDetail::~IStatusDetail() {}
DaneJoe::IStatusDetail::operator bool()const
{
    return is_ok();
}
bool DaneJoe::IStatusDetail::is_ok()const
{
    return m_status_level == StatusLevel::Ok;
}
bool DaneJoe::IStatusDetail::is_error()const
{
    return m_status_level == StatusLevel::Error;
}
bool DaneJoe::IStatusDetail::is_branch()const
{
    return m_status_level == StatusLevel::Branch;
}
DaneJoe::StatusLevel DaneJoe::IStatusDetail::get_status_level()const
{
    return m_status_level;
}
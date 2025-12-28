#include "danejoe/database/i_sql_stringify.hpp"

void DaneJoe::ISqlStringify::set_sign_config(const SqlSignConfig& config)
{
    m_sign_config = config;
}

DaneJoe::SqlSignConfig DaneJoe::ISqlStringify::get_sign_config()
{
    return m_sign_config;
}

int64_t DaneJoe::to_time_ms(const std::chrono::system_clock::time_point& time)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
}
int64_t DaneJoe::to_time_ms(const std::chrono::steady_clock::time_point& time)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
}

template<>
std::chrono::system_clock::time_point DaneJoe::from_time_ms(int64_t time)
{
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(time));
}
template<>
std::chrono::steady_clock::time_point DaneJoe::from_time_ms(int64_t time)
{
    return std::chrono::steady_clock::time_point(std::chrono::milliseconds(time));
}
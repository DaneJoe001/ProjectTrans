
#include "danejoe/network/status/posix_status_code.hpp"

DaneJoe::StatusCode DaneJoe::make_posix_status_code(
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    auto status_detail = make_posix_status_detail(user_message, branch_set);
    return StatusCode(status_detail);
}

DaneJoe::StatusCode DaneJoe::make_posix_status_code(
    bool is_ok,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    auto status_detail = make_posix_status_detail(is_ok, user_message, branch_set);
    return StatusCode(status_detail);
}

DaneJoe::StatusCode DaneJoe::make_posix_status_code(
    int error_code,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    auto status_detail = make_posix_status_detail(error_code, user_message, branch_set);
    return StatusCode(status_detail);
}

DaneJoe::StatusCode DaneJoe::make_posix_status_code(
    StatusLevel status_level,
    std::optional<std::string> user_message,
    PosixStatusSet branch_set)
{
    auto status_detail = make_posix_status_detail(status_level, user_message, branch_set);
    return StatusCode(status_detail);
}




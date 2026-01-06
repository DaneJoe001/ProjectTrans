#pragma once

#include <optional>
#include <string>

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/status/posix_status_detail.hpp"


namespace DaneJoe
{
    StatusCode make_posix_status_code(
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    StatusCode make_posix_status_code(
        bool is_ok,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    StatusCode make_posix_status_code(
        int error_code,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    StatusCode make_posix_status_code(
        StatusLevel status_level,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());
}
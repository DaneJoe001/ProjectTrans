/**
 * @file posix_status_code.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief POSIX 状态码构造工具
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供 make_posix_status_code() 一组重载，用于将 POSIX/errno 相关信息转换为统一的 StatusCode。
 *          可选的 user_message 用于附加用户侧错误说明；branch_set 用于携带更细分的 POSIX 状态分支信息。
 */
#pragma once

#include <optional>
#include <string>

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/status/posix_status_detail.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    /**
     * @brief 构造 POSIX 状态码
     * @param user_message 可选的用户提示信息
     * @param branch_set POSIX 状态分支集合
     * @return 构造得到的 StatusCode
     * @details 默认根据当前 POSIX 错误状态（通常对应 errno）构造 StatusCode。
     */
    StatusCode make_posix_status_code(
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 根据 is_ok 构造 POSIX 状态码
     * @param is_ok 若为 true 则返回 OK；若为 false 则根据当前 POSIX 错误状态（通常对应 errno）构造
     * @param user_message 可选的用户提示信息
     * @param branch_set POSIX 状态分支集合
     * @return 构造得到的 StatusCode
     */
    StatusCode make_posix_status_code(
        bool is_ok,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 根据指定 error_code 构造 POSIX 状态码
     * @param error_code POSIX 错误码（通常为 errno 值）
     * @param user_message 可选的用户提示信息
     * @param branch_set POSIX 状态分支集合
     * @return 构造得到的 StatusCode
     * @details 使用显式的 error_code 构造状态码，用于在 errno 可能已被覆盖的场景下保留原始错误信息。
     */
    StatusCode make_posix_status_code(
        int error_code,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 根据指定状态等级构造 POSIX 状态码
     * @param status_level 状态等级
     * @param user_message 可选的用户提示信息
     * @param branch_set POSIX 状态分支集合
     * @return 构造得到的 StatusCode
     * @details 用于在无明确 errno 的情况下，按给定状态等级构造 StatusCode，并携带 POSIX 分支信息。
     */
    StatusCode make_posix_status_code(
        StatusLevel status_level,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());
#endif
}
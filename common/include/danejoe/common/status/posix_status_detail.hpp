/**
 * @file posix_status_detail.hpp
 * @brief POSIX 状态详情
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供基于 POSIX errno 的状态详情实现（PosixStatusDetail），用于把 errno 映射为 StatusLevel
 *          并提供可读错误消息。
 *          注意：读取 errno 的构造函数必须紧贴系统调用失败现场，否则 errno 可能已被覆盖。
 */
#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <string_view>

#include "danejoe/common/status/i_status_detail.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class PosixStatusSet
     * @brief POSIX 状态码集合
     * @details 用于指定哪些 errno 值应被视为“Branch”（可分支处理）而非“Error”。
     */
    class PosixStatusSet
    {
    public:
        /**
         * @brief 构造空集合
         */
        PosixStatusSet();
        /**
         * @brief 从集合构造
         * @param set 状态码集合
         */
        PosixStatusSet(std::vector<int> set);
        /**
         * @brief 是否包含指定状态码
         * @param status_code 状态码
         * @return 是否包含
         */
        bool has_status_code(int status_code)const;
    private:
        /// @brief 状态码集合
        std::vector<int> m_set;
    };

    /**
     * @class PosixStatusDetail
     * @brief POSIX 状态详情
     * @details 基于 errno / 错误码生成 StatusLevel 与消息。
     *          支持通过 branch_set 将部分 errno 归类为 Branch。
     */
    class PosixStatusDetail :public IStatusDetail
    {
    public:
        /**
         * @brief 构造函数
         * @note 捕获当前 errno 并映射为 StatusLevel（Ok/Branch/Error）
         * @note 必须紧贴 syscall 失败现场调用，否则 errno 可能已被覆盖
         * @note user_message 若提供，将覆盖系统 strerror(errno)
         * @param user_message 用户自定义消息（可选）
         * @param branch_set 分支集合：命中时映射为 Branch
         */
        PosixStatusDetail(
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @param is_ok true代表完全正确,false代表不存在分支处理价值的错误
         * @note 不读取 errno（用于逻辑/防御性错误）
         * @param user_message 用户自定义消息（可选）
         * @param branch_set 分支集合：命中时映射为 Branch
         */
        PosixStatusDetail(
            bool is_ok,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @note 直接指定 errno/status_code，并映射为 StatusLevel
         * @note user_message 若提供，将覆盖系统 strerror(status_code)
         * @param error_code 错误码
         * @param user_message 用户自定义消息（可选）
         * @param branch_set 分支集合：命中时映射为 Branch
         */
        PosixStatusDetail(
            int error_code,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @note 仅指定 StatusLevel（用于逻辑/防御性分支，不读取 errno）
         * @param status_level 状态等级
         * @param user_message 用户自定义消息（可选）
         * @param branch_set 分支集合：命中时映射为 Branch
         */
        PosixStatusDetail(
            StatusLevel status_level,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());

        /**
         * @brief 获取可读消息
         * @return 消息字符串
         */
        std::string message()const override;
        /**
         * @brief 获取状态域
         * @return 域名
         */
        std::string_view domain()const override;
        /**
         * @brief 比较是否相等
         * @param rhs 右操作数
         * @return 是否相等
         */
        bool operator==(const IStatusDetail& rhs) const override;
    private:
        /**
         * @brief 将错误码映射为状态等级
         * @param error_code 错误码
         * @return 状态等级
         */
        StatusLevel to_status_level(int error_code)const;
    private:
        /// @brief 域名
        const char* m_domain = "PosixStatusDetail";
        /// @brief 分支集合
        PosixStatusSet m_branch_set;
        /// @brief 错误码（errno）
        std::optional<int> m_error_code = std::nullopt;
        /// @brief 用户自定义消息（优先于系统错误消息）
        std::optional<std::string> m_user_message = std::nullopt;
    };

    /**
     * @brief 构造 POSIX 状态详情（捕获当前 errno）
     * @param user_message 用户自定义消息（可选）
     * @param branch_set 分支集合：命中时映射为 Branch
     * @return 状态详情对象
     */
    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 构造 POSIX 状态详情（不读取 errno）
     * @param is_ok 是否为 Ok
     * @param user_message 用户自定义消息（可选）
     * @param branch_set 分支集合：命中时映射为 Branch
     * @return 状态详情对象
     */
    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        bool is_ok,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 构造 POSIX 状态详情（指定错误码）
     * @param error_code 错误码
     * @param user_message 用户自定义消息（可选）
     * @param branch_set 分支集合：命中时映射为 Branch
     * @return 状态详情对象
     */
    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        int error_code,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    /**
     * @brief 构造 POSIX 状态详情（指定状态等级）
     * @param status_level 状态等级
     * @param user_message 用户自定义消息（可选）
     * @param branch_set 分支集合：命中时映射为 Branch
     * @return 状态详情对象
     */
    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        StatusLevel status_level,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());
};
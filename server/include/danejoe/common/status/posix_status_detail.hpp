#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <string_view>

#include "danejoe/common/status/i_status_detail.hpp"

namespace DaneJoe
{
    class PosixStatusSet
    {
    public:
        PosixStatusSet();
        PosixStatusSet(std::vector<int> set);
        bool has_status_code(int status_code)const;
    private:
        std::vector<int> m_set;
    };
    class PosixStatusDetail :public IStatusDetail
    {
    public:
        /**
         * @brief 构造函数
         * @note 捕获当前 errno 并映射为 StatusLevel（Ok/Branch/Error）
         * @note 必须紧贴 syscall 失败现场调用，否则 errno 可能已被覆盖
         * @note user_message 若提供，将覆盖系统 strerror(errno)
         */
        PosixStatusDetail(
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @param is_ok true代表完全正确,false代表不存在分支处理价值的错误
         * @note 不读取 errno（用于逻辑/防御性错误）
         */
        PosixStatusDetail(
            bool is_ok,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @note 直接指定 errno/status_code，并映射为 StatusLevel
         * @note user_message 若提供，将覆盖系统 strerror(status_code)
         */
        PosixStatusDetail(
            int error_code,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        /**
         * @brief 构造函数
         * @note 仅指定 StatusLevel（用于逻辑/防御性分支，不读取 errno）
         */
        PosixStatusDetail(
            StatusLevel status_level,
            std::optional<std::string> user_message = std::nullopt,
            PosixStatusSet branch_set = PosixStatusSet());
        std::string message()const override;
        std::string_view domain()const override;
        bool operator==(const IStatusDetail& rhs) const override;
    private:
        StatusLevel to_status_level(int error_code)const;
    private:
        const char* m_domain = "PosixStatusDetail";
        PosixStatusSet m_branch_set;
        std::optional<int> m_error_code = std::nullopt;
        std::optional<std::string> m_user_message = std::nullopt;
    };
    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        bool is_ok,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        int error_code,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());

    std::shared_ptr<PosixStatusDetail> make_posix_status_detail(
        StatusLevel status_level,
        std::optional<std::string> user_message = std::nullopt,
        PosixStatusSet branch_set = PosixStatusSet());
};
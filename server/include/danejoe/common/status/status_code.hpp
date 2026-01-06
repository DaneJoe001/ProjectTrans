#pragma once

#include <string>
#include <memory>

#include "danejoe/common/status/i_status_detail.hpp"

namespace DaneJoe
{
    /**
     * @class StatusCode
     * @brief 状态码
     * @note is_valid判断状态信息是否有效
     * @note 默认构造为 Unknown/Invalid 占位（detail为空）
     * @note 建议：正常返回路径尽量构造明确的 detail（Ok/Branch/Error/Unknown + message），避免吞错
     * @note false情况下需要区分error和branch
     * @note 仅在 detail 为 nullptr 时为 invalid，否则视为 valid
     */
    class StatusCode
    {
    public:
        StatusCode();
        StatusCode(std::shared_ptr<IStatusDetail> error_code);
        std::string message()const;
        bool is_ok()const;
        bool is_error()const;
        bool is_branch()const;
        /**
         * @brief StatusCode 是否携带有效 detail（否则就是 Unknown/Invalid 占位）
         * @note 仅在 detail 为 nullptr 时为 invalid，否则视为 valid
         * @note 用于区分是否需要进一步处理错误信息
         */
        bool is_valid()const;
        StatusLevel get_status_level()const;
        /**
         * @note false 情况：必须区分 Branch/Error（不要把非 Ok 都当 Error）
         */
        explicit operator bool()const;
        bool operator==(const StatusCode& other)const;
        bool operator==(std::shared_ptr<IStatusDetail> other_detail);
    private:
        std::shared_ptr<IStatusDetail> m_status_detail = nullptr;
    };
};
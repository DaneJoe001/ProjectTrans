/**
 * @file status_code.hpp
 * @brief 状态码
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供 StatusCode 类型，用于携带状态详情（IStatusDetail）并对外提供统一的 ok/branch/error 判断与消息获取接口。
 *          StatusCode 通过持有 std::shared_ptr<IStatusDetail> 管理状态详情对象；当 detail 为空时表示“无效占位”。
 */
#pragma once

#include <string>
#include <memory>

#include "danejoe/common/status/i_status_detail.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
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
        /**
         * @brief 默认构造
         * @details 默认构造为 Unknown/Invalid 占位（detail 为空）。
         */
        StatusCode();
        /**
         * @brief 从状态详情构造
         * @param error_code 状态详情对象
         */
        StatusCode(std::shared_ptr<IStatusDetail> error_code);
        /**
         * @brief 获取可读消息
         * @return 消息字符串
         */
        std::string message()const;
        /**
         * @brief 是否为 Ok
         * @return true 为 Ok
         * @return false 非 Ok
         */
        bool is_ok()const;
        /**
         * @brief 是否为 Error
         * @return true 为 Error
         * @return false 非 Error
         */
        bool is_error()const;
        /**
         * @brief 是否为 Branch
         * @return true 为 Branch
         * @return false 非 Branch
         */
        bool is_branch()const;
        /**
         * @brief StatusCode 是否携带有效 detail（否则就是 Unknown/Invalid 占位）
         * @note 仅在 detail 为 nullptr 时为 invalid，否则视为 valid
         * @note 用于区分是否需要进一步处理错误信息
         */
        bool is_valid()const;
        /**
         * @brief 获取状态等级
         * @return 状态等级
         * @details 返回状态码对应的等级。
         */
        StatusLevel get_status_level()const;
        /**
         * @note false 情况：必须区分 Branch/Error（不要把非 Ok 都当 Error）
         */
        explicit operator bool()const;
        /**
         * @brief 比较两个 StatusCode 是否相等
         * @param other 右操作数
         * @return 是否相等
         * @details 比较两个状态码是否相等。
         */
        bool operator==(const StatusCode& other)const;
        /**
         * @brief 与指定状态详情比较
         * @param other_detail 右操作数
         * @return 是否相等
         * @details 比较状态码与指定状态详情是否相等。
         */
        bool operator==(std::shared_ptr<IStatusDetail> other_detail);
    private:
        /// @brief 状态详情对象
        std::shared_ptr<IStatusDetail> m_status_detail = nullptr;
    };
};
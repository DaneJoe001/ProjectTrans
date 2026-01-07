/**
 * @file i_status_detail.hpp
 * @brief 状态详情接口
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义状态详情（Status detail）的抽象接口，用于为状态码提供更丰富的错误/分支信息描述。
 *          该接口通常由具体平台或子系统实现，并由上层 StatusCode/Result 等类型持有或引用。
 */
#pragma once

#include <string>
#include <string_view>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum StatusLevel
     * @brief 状态等级
     * @details 描述状态的粗粒度分类：成功（Ok）、分支（Branch）、错误（Error）以及未知（Unknown）。
     */
    enum class StatusLevel
    {
        /// @brief 成功
        Ok,
        /// @brief 分支（非错误的控制流结果）
        Branch,
        /// @brief 错误
        Error,
        /// @brief 未知
        Unknown
    };
    /**
     * @class IStatusDetail
     * @brief 状态详情接口
     * @details 提供状态的可读消息、来源域（domain）以及等级判断。
     *          实现类需保证比较运算符的自反性与一致性。
     */
    class IStatusDetail
    {
    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~IStatusDetail() = 0;
        /**
         * @brief 获取可读消息
         * @return 消息字符串
         */
        virtual std::string message()const = 0;
        /**
         * @brief 获取状态域
         * @return 域名（如模块/子系统标识）
         */
        virtual std::string_view domain()const = 0;
        /**
         * @brief 比较两个状态详情是否相等
         * @param rhs 右操作数
         * @return 是否相等
         */
        virtual bool operator==(const IStatusDetail& rhs) const = 0;
        /**
         * @brief 显式布尔转换
         * @return true 表示状态为 Ok
         * @return false 表示状态非 Ok
         */
        explicit operator bool()const;
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
         * @brief 获取状态等级
         * @return 状态等级
         */
        StatusLevel get_status_level()const;
    protected:
        /// @brief 状态等级
        StatusLevel m_status_level = StatusLevel::Unknown;
    };
}
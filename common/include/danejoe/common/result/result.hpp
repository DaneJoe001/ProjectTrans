/**
 * @file result.hpp
 * @brief 结果类型
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供轻量的 Result<T> 类型，用于在返回值中同时携带“可选值”和状态码。
 *          当 has_value() 为 false 时，value() 访问将触发异常（std::optional::value()）。
 */
#pragma once

#include <optional>
#include <utility>

#include "danejoe/common/status/status_code.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class Result
     * @brief 结果类型
     * @tparam T 值类型
     * @details 通过 std::optional<T> 存储可选值，并通过 StatusCode 描述结果状态。
     */
    template<class T>
    class Result
    {
    public:
        /**
         * @brief 构造结果
         * @param value 可选值
         * @param status_code 状态码
         */
        Result(std::optional<T> value, const StatusCode& status_code) :
            m_value(std::move(value)),
            m_status_code(status_code)
        {}
        /**
         * @brief 构造无值结果
         * @param status_code 状态码
         */
        Result(const StatusCode& status_code) :
            m_value(std::nullopt),
            m_status_code(status_code)
        {}
        /**
         * @brief 是否包含值
         * @return true 包含值
         * @return false 不包含值
         */
        bool has_value()const
        {
            return m_value.has_value();
        }
        /**
         * @brief 获取值（const&）
         * @return 值引用
         * @note 调用前需确保 has_value() == true，否则会抛出异常。
         */
        const T& value()const&
        {
            return m_value.value();
        }
        /**
         * @brief 获取值（&）
         * @return 值引用
         * @note 调用前需确保 has_value() == true，否则会抛出异常。
         */
        T& value()&
        {
            return m_value.value();
        }
        /**
         * @brief 获取值（&&）
         * @return 值右值引用
         * @note 调用前需确保 has_value() == true，否则会抛出异常。
         */
        T&& value()&&
        {
            return std::move(m_value.value());
        }
        /**
         * @brief 获取状态码
         * @return 状态码引用
         */
        const StatusCode& status_code()const
        {
            return m_status_code;
        }
    private:
        /// @brief 可选值
        std::optional<T> m_value = std::nullopt;
        /// @brief 状态码
        StatusCode m_status_code;
    };
};
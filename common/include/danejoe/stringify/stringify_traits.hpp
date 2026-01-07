/**
 * @file stringify_traits.hpp
 * @brief 字符串化特征工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 * @details 提供 stringify 相关的类型特征（traits），用于在编译期检测类型是否支持特定的字符串化方式。
 *          这些 traits 通常被 stringify_to_string.hpp 用于选择合适的 to_string 分支。
 */
#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <ostream>

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 判断类型是否支持 std::to_string
     * @tparam T 类型
     * @details 当表达式 std::to_string(std::declval<T>()) 合法时为 true。
     * @note has_std_to_string<T>::value 为 true 表示 T 支持 std::to_string。
     */
    template <typename T, typename = void>
    struct has_std_to_string : std::false_type {};
    /**
     * @brief has_std_to_string 的匹配分支：当 std::to_string 对 T 可用时为 true
     * @tparam T 类型
     */
    template <typename T>
    struct has_std_to_string<T,
        std::void_t<decltype(std::to_string(std::declval<T>()))>> : std::true_type {};
    /**
     * @brief 判断类型是否支持 to_string 成员函数
     * @tparam T 类型
     * @details 当表达式 std::declval<T>().to_string() 合法时为 true。
     * @note has_member_to_string<T>::value 为 true 表示 T 支持 to_string 成员函数。
     */
    template <typename T, typename = void>
    struct has_member_to_string : std::false_type {};
    /**
     * @brief has_member_to_string 的匹配分支：当 T 有 to_string 成员函数时为 true
     * @tparam T 类型
     */
    template <typename T>
    struct has_member_to_string<T,
        std::void_t<decltype(std::declval<T>().to_string())>> : std::true_type {};
    /**
     * @brief 检测类型是否支持流输出
     * @tparam T 类型
     * @details 当表达式 std::declval<std::ostream&>() << std::declval<const T&>() 合法时为 true。
     * @note has_stream_out<T>::value 为 true 表示 T 支持通过 operator<< 输出到 std::ostream。
     */
    template <typename T, typename = void>
    struct has_stream_out : std::false_type {};
    /**
     * @brief has_stream_out 的匹配分支：当 operator<< 对 T 可用时为 true
     * @tparam T 类型
     */
    template <typename T>
    struct has_stream_out<T,
        std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> : std::true_type {};
}

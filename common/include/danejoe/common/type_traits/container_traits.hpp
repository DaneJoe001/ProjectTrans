/**
 * @file container_traits.hpp
 * @brief 容器类型特征工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 * @details 提供用于类型萃取的基础设施，当前包含对“是否可迭代（iterable）”的静态检测。
 */
#pragma once

#include <type_traits>
#include <iterator>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 判断类型是否可使用 std::begin/std::end 进行迭代
     * @tparam T 待检测类型
     * @details 通过 SFINAE 检测 std::begin/std::end 对 T 是否可用。
     * @note has_iterator<T>::value 为 true 表示 T 满足可迭代（iterable）要求。
     */
    template <typename T, typename = void>
    struct has_iterator : std::false_type {};
    /**
     * @brief has_iterator 的匹配分支：当 std::begin/std::end 对 T 可用时为 true
     * @tparam T 待检测类型
     * @note 该特化在 std::begin/std::end 表达式均可实例化时匹配。
     */
    template <typename T>
    struct has_iterator<T, std::void_t<
        decltype(std::begin(std::declval<T>())),
        decltype(std::end(std::declval<T>()))>> : std::true_type{};
}

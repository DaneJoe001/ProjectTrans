/**
 * @file enum_type_traits.hpp
 * @brief 枚举类型特征工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 * @details 提供用于枚举类型的编译期特征工具。
 *          目前用于控制是否为枚举类型启用位标志位相关操作（参见 enum_flag.hpp）。
 */
#pragma once

#include <type_traits>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 启用位掩码运算符模板
     * @tparam T 待检测类型
     * @note enable_bitmask_operator<T>::value 为 true 表示 T 支持位掩码运算
     * @note 对于需要实现位运算的枚举类，需要手动特化该模板并设置为 true。
     * @note 通常与 enum_flag.hpp 配合使用：仅当该 trait 为 true 时才会启用 |/&/^ 等运算符重载。
     */
    template<typename T>
    struct enable_bitmask_operator :std::false_type {};
}

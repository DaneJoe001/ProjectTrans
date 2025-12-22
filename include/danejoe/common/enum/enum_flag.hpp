/**
 * @file enum_flag.hpp
 * @brief 枚举标志位工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 */
#pragma once

#include <type_traits>

#include "danejoe/common/type_traits/enum_type_traits.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 枚举或运算模板
     * @tparam Enum 枚举类型
     * @note std::enable_if_t用于选择是否启用模板
     * @note enable_bitmask_operator<Enum>::value用于选择是否启用位运算
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 枚举类型
     */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum & operator|=(Enum& lhs, const Enum& rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        lhs = static_cast<Enum>(static_cast<enum_type>(lhs) | static_cast<enum_type>(rhs));
        return lhs;
    }
    /**
    * @brief 枚举或运算模板
    * @tparam Enum 枚举类型
    * @note std::enable_if_t用于选择是否启用模板
    * @param lhs 左操作数
    * @param rhs 右操作数
    * @return 枚举类型
    */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum & operator&=(Enum& lhs, const Enum& rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        lhs = static_cast<Enum>(static_cast<enum_type>(lhs) & static_cast<enum_type>(rhs));
        return lhs;
    }
    /**
    * @brief 枚举或运算模板
    * @tparam Enum 枚举类型
    * @note 仅对 enable_bitmask_operator<Enum>::value 为 true 的枚举类型启用
    * @param lhs 左操作数
    * @param rhs 右操作数
    * @return 枚举类型
    */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum & operator^=(Enum& lhs, const Enum& rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        lhs = static_cast<Enum>(static_cast<enum_type>(lhs) ^ static_cast<enum_type>(rhs));
        return lhs;
    }
    /**
     * @brief 枚举或运算模板
     * @tparam Enum 枚举类型
     * @note std::enable_if_t用于选择是否启用模板
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 枚举类型
     */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum operator|(Enum lhs, Enum rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<enum_type>(lhs) | static_cast<enum_type>(rhs));
    }
    /**
     * @brief 枚举与运算模板
     * @tparam Enum 枚举类型
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 枚举类型
     */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum operator&(Enum lhs, Enum rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<enum_type>(lhs) & static_cast<enum_type>(rhs));
    }
    /**
     * @brief 枚举异或运算模板
     * @tparam Enum 枚举类型
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 枚举类型
     */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        Enum operator^(Enum lhs, Enum rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<enum_type>(lhs) ^ static_cast<enum_type>(rhs));
    }
    /**
     * @brief 判断目标标志集是否包含源标志集
     * @tparam Enum 枚举类型
     * @param dest_set 需要存在的目标标志集
     * @param src_set 需要检查的源标志集
     * @return true 目标标志集包含源标志集
     * @return false 目标标志集不包含源标志集
     */
    template<class Enum, typename =
        std::enable_if_t<std::is_enum<Enum>::value&&
        enable_bitmask_operator<Enum>::value>>
        bool has_flag(Enum dest_set, Enum src_set)
    {
        return (src_set & dest_set) == src_set;
    }
}

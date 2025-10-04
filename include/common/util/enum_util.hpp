#pragma once

#include <type_traits>
#include <cstdint>
#include <vector>
#include <string>


namespace DaneJoe
{
    /**
     * @brief 枚举或运算模板
     * @tparam Enum 枚举类型
     * @note std::enable_if_t用于选择是否启用模板
     * @param lhs 左操作数
     * @param rhs 右操作数
     */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
    Enum& operator|=(Enum& lhs, const Enum& rhs)noexcept
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
    */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
    Enum& operator&=(Enum& lhs, const Enum& rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        lhs = static_cast<Enum>(static_cast<enum_type>(lhs) & static_cast<enum_type>(rhs));
        return lhs;
    }
    /**
    * @brief 枚举或运算模板
    * @tparam Enum 枚举类型
    * @note std::enable_if_t用于选择是否启用模板
    * @param lhs 左操作数
    * @param rhs 右操作数
    */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
    Enum& operator^=(Enum& lhs, const Enum& rhs)noexcept
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
     */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
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
     */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
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
     */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
    Enum operator^(Enum lhs, Enum rhs)noexcept
    {
        /// 获取枚举底层类型
        using enum_type = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<enum_type>(lhs) ^ static_cast<enum_type>(rhs));
    }
    /**
     * @brief 判断是否有特定标志
     * @tparam Enum 枚举类型
     * @param flag 需要检查的标志
     * @param rhs 标志集
     */
    template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
    bool has_flag(Enum flag_set, Enum flag)
    {
        return (flag & flag_set) == flag;
    }
}
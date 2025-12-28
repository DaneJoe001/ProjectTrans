/**
 * @file enum_convert.hpp
 * @brief 枚举转换工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-19
 */
#pragma once

#include <string>
#include <cstdint>

/**
 * @brief 枚举转字符串(和参数保持一致)
 * @param enum 枚举类型
 * @return 和调用参数名称一致字符串
 * @note 仅用于库内部实现 to_string / enum_cast，不对外保证字符串兼容性
 */
#define ENUM_TO_STRING(enum) #enum

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 将字符串转换为枚举值
     * @tparam T 枚举类型
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值
     */
    template <typename T>
    T enum_cast(const std::string& enum_string) = delete;

    /**
     * @brief 将整数值转换为枚举值
     * @tparam T 枚举类型
     * @param enum_value 枚举整数值
     * @return 对应的枚举值
     */
    template <typename T>
    T enum_cast(int64_t enum_value) = delete;
}

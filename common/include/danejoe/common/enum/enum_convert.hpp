/**
 * @file enum_convert.hpp
 * @brief 枚举转换工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-19
 * @details 提供枚举与字符串/整数之间转换的基础设施。
 *          其中 enum_cast<T>(...) 采用“声明为 delete + 显式特化”的方式：仅对显式特化的枚举类型可用。
 *          本模块主要用于库内部实现 to_string / enum_cast，不对外保证字符串兼容性。
 */
#pragma once

#include <string>
#include <cstdint>

 /**
  * @def ENUM_TO_STRING
  * @brief 枚举转字符串（与参数 token 保持一致）
  * @param enum_value 枚举 token
  * @return 与传入 token 名称一致的字符串
  * @note 该宏仅用于生成“枚举项名称字符串”，不等同于枚举值的用户可读描述。
  * @note 仅用于库内部实现 to_string / enum_cast，不对外保证字符串兼容性。
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
     * @note 该模板默认被声明为 delete，需为具体枚举类型提供显式特化。
     */
    template <typename T>
    T enum_cast(const std::string& enum_string) = delete;

    /**
     * @brief 将整数值转换为枚举值
     * @tparam T 枚举类型
     * @param enum_value 枚举整数值
     * @return 对应的枚举值
     * @note 该模板默认被声明为 delete，需为具体枚举类型提供显式特化。
     */
    template <typename T>
    T enum_cast(int64_t enum_value) = delete;
}

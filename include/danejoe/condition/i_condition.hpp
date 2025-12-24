/**
 * @file i_condition.hpp
 * @brief 条件接口
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 */
#pragma once

#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe
  */
namespace DaneJoe
{
    /**
     * @brief 方程式方向
     */
    enum class SignDirection
    {
        /// @brief 小到大的不等式方向
        SmallToLarge,
        /// @brief 大到小的不等式方向
        LargeToSmall,
        /// @brief 左变量方程式方向
        VarLeft,
        /// @brief 右变量方程式方向
        VarRight,
        /// @brief 未知方向
        Unknown
    };
    /**
     * @brief 条件关系
     */
    enum class ConditionRelation
    {
        /// @brief 或关系
        Or,
        /// @brief 与关系
        And,
        /// @brief 异或关系
        Xor,
        /// @brief 非关系
        Not,
        /// @brief 未知关系
        Unknown
    };
    /**
     * @brief 将方程式方向转换为字符串
     * @param direction 方程式方向
     * @return 对应的枚举字符串
     */
    std::string to_string(SignDirection direction);
    /**
     * @brief 将条件关系转换为字符串
     * @param relation 条件关系
     * @return 对应的枚举字符串
     */
    std::string to_string(ConditionRelation relation);
    /**
     * @brief 从字符串转换为方程式方向
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 SignDirection::Unknown
     */
    template<>
    SignDirection enum_cast<SignDirection>(const std::string& enum_string);
    /**
     * @brief 从字符串转换为条件关系
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 ConditionRelation::Unknown
     */
    template<>
    ConditionRelation enum_cast<ConditionRelation>(const std::string& enum_string);
    /**
     * @struct ICondition
     * @brief 条件接口
     */
    struct ICondition
    {
        /**
         * @brief 析构函数
         */
        virtual ~ICondition() {}
        /**
         * @brief 清空条件
         */
        virtual void clear() = 0;
    };
}

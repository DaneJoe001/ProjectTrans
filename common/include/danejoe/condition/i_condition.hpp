/**
 * @file i_condition.hpp
 * @brief 条件接口
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 * @details 定义条件表达与组合所需的基础枚举与接口。
 *          SignDirection 用于描述不等式的方向/变量位置；ConditionRelation 用于描述条件之间的逻辑关系。
 *          ICondition 为条件对象的最小抽象接口，提供清空内部状态的统一入口。
 */
#pragma once

#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 方程式方向
     * @details 用于描述不等式表达式的方向与变量位置。
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
     * @details 用于描述多个条件之间的组合关系（逻辑或/与/异或/非）。
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
     * @details 条件对象的基础接口。
     *          实现类应在 clear() 中将对象恢复到“未设置/无条件”的初始状态。
     */
    struct ICondition
    {
        /**
         * @brief 析构函数
         */
        virtual ~ICondition() {}
        /**
         * @brief 清空条件
         * @details 将条件对象重置为初始状态。
         */
        virtual void clear() = 0;
    };
}

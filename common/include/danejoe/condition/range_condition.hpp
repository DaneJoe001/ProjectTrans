/**
 * @file range_condition.hpp
 * @brief 范围条件
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 * @details 定义范围条件抽象接口 IRangeCondition，以及基于区间集合的 RangeCondition<T> 实现。
 *          RangeCondition<T> 以一组 SingleInterval<T> 表达范围集合（区间之间为并列/或关系）。
 *          通过 add_range(..., ConditionRelation) 支持与/或/非等逻辑组合，并在内部进行 simplify() 合并相邻或可并区间。
 */
#pragma once

#include <cstdint>
#include <sstream>
#include <set>
#include <vector>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/condition/single_interval.hpp"
#include "danejoe/common/core/variable_util.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 范围条件接口
     * @details 提供将范围条件转换为整数区间集合的统一抽象。
     *          该接口继承自 ICondition，并要求实现 clear() 的重置语义。
     */
    class IRangeCondition : public ICondition
    {
    public:
        /**
         * @brief 获取整数区间集合
         * @return 可转换为整数表示的区间集合
         * @details 用于将范围条件提升/转换为整数区间表示。
         *          若部分区间无法转换为整数区间（取决于 SingleInterval<T> 的实现），则这些区间将被忽略。
         */
        virtual std::vector<SingleInterval<int64_t>> get_integer_intervals()const = 0;
    };
    /**
     * @brief 范围条件
     * @tparam T 范围类型
     * @details 以 std::set<SingleInterval<T>> 存储范围集合，集合内部的多个区间表示“或(OR)”关系。
     *          - add_range(..., And) 计算当前集合与 new_interval 的交集
     *          - add_range(..., Or) 追加 new_interval 到集合
     *          - add_range(..., Not) 从当前集合中剔除 new_interval 覆盖的部分
     *          add_range 完成后会调用 simplify() 以合并可并区间，保证集合内区间尽量简化。
     */
    template<typename T>
    class RangeCondition : public IRangeCondition
    {
    public:
        /**
         * @brief 默认构造函数
         * @details 构造空范围集合。
         */
        RangeCondition() {}
        /**
         * @brief 构造函数
         * @param value 单个值
         * @details 以单点区间构造范围集合。
         */
        RangeCondition(const T& value)
        {
            SingleInterval<T> new_interval(value);
            m_intervals.insert(new_interval);
        }
        /**
         * @brief 构造函数
         * @param value 单个范围
         */
        RangeCondition(const SingleInterval<T>& value)
        {
            if (value.is_empty())
            {
                return;
            }
            m_intervals.insert(SingleInterval<T>(value));
        }
        /**
         * @brief 获取范围集合
         * @return std::set<SingleInterval<T>> 范围集合
         */
        std::set<SingleInterval<T>> get_intervals()const
        {
            return m_intervals;
        }
        /**
         * @brief 添加范围
         * @param new_interval 新范围
         * @param relation 关系
         * @details 根据 relation 对当前范围集合进行逻辑组合，并在处理后调用 simplify()。
         */
        void add_range(
            const SingleInterval<T>& new_interval,
            ConditionRelation relation)
        {
            switch (relation)
            {
                case ConditionRelation::And:
                    and_single_interval(new_interval);
                    break;
                case ConditionRelation::Or:
                    or_single_interval(new_interval);
                    break;
                case ConditionRelation::Not:
                    not_single_interval(new_interval);
                    break;
                default: break;
            }
            simplify();
        }
        /**
         * @brief 添加范围
         * @param new_range 新范围
         * @param relation 关系
         */
        void add_range(
            const RangeCondition<T>& new_range,
            ConditionRelation relation)
        {
            for (auto new_interval : new_range.get_intervals())
            {
                add_range(new_interval, relation);
            }
        }
        /**
         * @brief 添加范围
         * @param new_value 新值
         * @param relation 关系
         */
        void add_range(
            const T& new_value,
            ConditionRelation relation)
        {
            SingleInterval<T> new_interval(new_value);
            add_range(new_interval, relation);
        }
        /**
         * @brief 或上单个范围
         * @param new_interval 新范围
         */
        void or_single_interval(const SingleInterval<T>& new_interval)
        {
            m_intervals.insert(new_interval);
        }
        /**
         * @brief 与上单个范围
         * @param new_interval 新范围
         */
        void and_single_interval(const SingleInterval<T>& new_interval)
        {
            std::set<SingleInterval<T>> result;
            for (auto interval : m_intervals)
            {
                auto intersect_interval =
                    interval.try_get_intersect(new_interval);
                if (intersect_interval.has_value())
                {
                    result.insert(intersect_interval.value());
                }
            }
            m_intervals = result;
        }
        /**
         * @brief 判断范围条件是否为空（可能包含空集条件）
         * @return true 为空
         * @return false 不为空
         */
        bool is_empty()const
        {
            return m_intervals.empty();
        }
        /**
         * @brief 非上单个范围
         * @param new_interval 新范围
         */
        void not_single_interval(const SingleInterval<T>& new_interval)
        {
            std::set<SingleInterval<T>> result;
            for (auto interval : m_intervals)
            {
                auto difference_intervals =
                    interval.get_difference(new_interval);
                for (auto difference_interval : difference_intervals)
                {
                    result.insert(difference_interval);
                }
            }
            m_intervals = result;
        }
        /**
         * @brief 尝试将范围简化,确保没有重叠区域
         * @details 通过 try_get_union() 将相邻或可并的区间合并，并移除空区间。
         */
        void simplify()
        {
            if (m_intervals.size() < 1)
            {
                return;
            }
            else if (m_intervals.size() == 1)
            {
                auto it = m_intervals.begin();
                if (it->is_empty())
                {
                    m_intervals.clear();
                }
                return;
            }
            std::set<SingleInterval<T>> result;
            auto it = m_intervals.begin();
            auto end_it = m_intervals.end();
            SingleInterval<T> current = *it;
            ++it;
            for (; it != end_it; ++it)
            {
                const auto& next = *it;
                if (current.is_empty())
                {
                    current = next;
                    continue;
                }
                auto merged = current.try_get_union(next);
                if (merged.has_value())
                {
                    current = merged.value();
                }
                else
                {
                    // current 已经不能再和 next 合并了，输出一段
                    if (!current.is_empty())
                    {
                        result.insert(current);
                    }
                    current = next;
                }
            }
            // 处理最后一段
            if (!current.is_empty())
            {
                result.insert(current);
            }
            m_intervals = std::move(result);
        }
        /**
         * @brief 获取两个范围集合的交集
         * @param other 其他范围
         * @return RangeCondition<T> 交集范围
         */
        RangeCondition<T> get_intersect_range(const RangeCondition<T>& other)const
        {
            RangeCondition<T> result;
            for (auto other_interval : other.get_intervals())
            {
                RangeCondition<T> temp = *this;
                temp.add_range(other_interval, ConditionRelation::And);
                result.add_range(temp, ConditionRelation::Or);
            }
            return result;
        }
        /**
         * @brief 获取两个范围集合的并集
         * @param other 其他范围
         * @return RangeCondition<T> 并集范围
         */
        RangeCondition<T>
            get_union_range(const RangeCondition<T>& other) const
        {
            RangeCondition<T> new_range = *this;
            for (auto other_interval : other.get_intervals())
            {
                new_range.add_range(other_interval, ConditionRelation::Or);
            }
            return new_range;
        }
        /**
         * @brief 获取两个范围集合的差集
         * @param other 其他范围
         * @return RangeCondition<T> 差集范围
         */
        RangeCondition<T> get_difference_range(const RangeCondition<T>& other)const
        {
            RangeCondition<T> result = *this;
            for (auto interval : other.m_intervals)
            {
                result.add_range(interval, ConditionRelation::Not);
            }
            return result;
        }
        /**
         * @brief 判断两个范围是否相等
         * @param other 其他范围
         * @return true 相等
         * @return false 不相等
         */
        bool is_equal(const RangeCondition<T>& other)const
        {
            return m_intervals == other.m_intervals;
        }
        /**
         * @brief 判断两个范围是否相交
         * @param other 其他范围
         * @return true 相交
         * @return false 不相交
         */
        bool is_intersect(const RangeCondition<T>& other)const
        {
            for (auto interval : other.m_intervals)
            {
                for (auto other_interval : m_intervals)
                {
                    if (interval.is_intersect(other_interval))
                        return true;
                }
            }
            return false;
        }
        /**
         * @brief 判断 other 是否完全落在当前范围集合内（包含边界）
         * @param other 其他范围
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool is_in_range(const SingleInterval<T>& other)const
        {
            for (auto interval : m_intervals)
            {
                if (interval.is_in_range(other))
                    return true;
            }
            return false;
        }
        /**
         * @brief 判断一个值是否在范围内
         * @param other 其他值
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool is_in_range(const T& other)const
        {
            for (auto interval : m_intervals)
            {
                if (interval.is_in_range(other))
                    return true;
            }
            return false;
        }
        /**
         * @brief 判断一个范围是否在另一个范围内
         * @param other 其他范围
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool is_in_range(const RangeCondition<T>& other)const
        {
            for (auto other_interval : other.m_intervals)
            {
                if (!is_in_range(other_interval))
                {
                    return false;
                }
            }
            return true;
        }
        /**
         * @brief 获取范围内的值
         * @param values 值列表
         * @return std::vector<T> 范围内的值
         */
        std::vector<T> get_values_in_range(const std::vector<T>& values)const
        {
            std::vector<T> result;
            for (auto value : values)
            {
                if (is_in_range(value))
                {
                    result.push_back(value);
                }
            }
            return result;
        }
        /**
         * @brief 获取不在范围内的值
         * @param values 值列表
         * @return std::vector<T> 不在范围内的值
         */
        std::vector<T> get_values_not_in_range(const std::vector<T>& values)const
        {
            std::vector<T> result;
            for (auto value : values)
            {
                if (!is_in_range(value))
                {
                    result.push_back(value);
                }
            }
            return result;
        }
        /**
         * @brief 清空范围条件
         */
        void clear()override
        {
            m_intervals.clear();
        }
        /**
         * @brief 转换为字符串
         * @param variable_name 变量名
         * @return std::string 字符串
         * @details 以 "RangeCondition: { interval1 || interval2 || ... }" 形式输出。
         */
        std::string to_string(const std::string& variable_name = "x") const
        {
            DANEJOE_UNUSED(variable_name)
                std::stringstream ss;
            ss << "RangeCondition: {";
            if (!m_intervals.empty())
            {
                bool is_first = true;
                for (auto begin = m_intervals.begin();
                    begin != m_intervals.end(); begin++)
                {
                    if (!is_first)
                    {
                        ss << "||";
                    }
                    is_first = false;
                    ss << " " << begin->to_string() << " ";
                }
            }
            ss << "}";
            return ss.str();
        }
        /**
         * @brief 判断两个范围是否相等
         * @param other 其他范围
         * @return true 相等
         * @return false 不相等
         */
        bool operator==(const RangeCondition<T>& other)const
        {
            return is_equal(other);
        }
        /**
         * @brief 判断两个范围是否不相等
         * @param other 其他范围
         * @return true 不相等
         * @return false 相等
         */
        bool operator!=(const RangeCondition<T>& other)const
        {
            return !is_equal(other);
        }
        /**
         * @brief 判断一个值是否在范围内
         * @param value 值
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool operator()(const T& value)const
        {
            return is_in_range(value);
        }
        /**
         * @brief 判断一个范围是否在另一个范围内
         * @param other 其他范围
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool operator()(const RangeCondition<T>& other)const
        {
            return is_in_range(other);
        }
        /**
         * @brief 判断一个值列表是否在范围内
         * @param values 值列表
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool operator()(const std::vector<T>& values)const
        {
            for (auto value : values)
            {
                if (!is_in_range(value))
                    return false;
            }
            return true;
        }
        /**
         * @copydoc IRangeCondition::get_integer_intervals
         */
        std::vector<SingleInterval<int64_t>> get_integer_intervals()const override;
    private:
        /// @brief 区间集合，并列关系
        std::set<SingleInterval<T>> m_intervals;
    };
    /**
     * @brief 计算 lhs 相对于 rhs 的差集（相对补集）
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 补集范围
     */
    template <typename T>
    RangeCondition<T>
        get_complement_range(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return lhs.get_difference_range(rhs);
    }
    /**
     * @brief 获取两个范围集合的交集
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 交集范围
     */
    template <typename T>
    RangeCondition<T>
        get_intersect_range(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return lhs.get_intersect_range(rhs);
    }
    /**
     * @brief 获取两个范围集合的并集
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 并集范围
     */
    template <typename T>
    RangeCondition<T>
        get_union_range(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return lhs.get_union_range(rhs);
    }
    /**
     * @brief 获取两个范围集合的差集
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 差集范围
     */
    template <typename T>
    RangeCondition<T>
        get_difference_range(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return lhs.get_difference_range(rhs);
    }
    /**
     * @brief 或上两个范围集合
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 或上后的范围
     */
    template <typename T>
    RangeCondition<T> operator||(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return get_union_range(lhs, rhs);
    }
    /**
     * @brief 与上两个范围集合
     * @tparam T 范围类型
     * @param lhs 左范围
     * @param rhs 右范围
     * @return RangeCondition<T> 与上后的范围
     */
    template <typename T>
    RangeCondition<T> operator&&(
        const RangeCondition<T>& lhs,
        const RangeCondition<T>& rhs)
    {
        return get_intersect_range(lhs, rhs);
    }
    template <typename T>
    std::vector<SingleInterval<int64_t>> RangeCondition<T>::get_integer_intervals()const
    {
        std::vector<SingleInterval<int64_t>> result;
        for (auto interval : m_intervals)
        {
            auto integer_interval = interval.get_integer_interval();
            if (integer_interval.has_value())
            {
                result.push_back(integer_interval.value());
            }
        }
        return result;
    }
}

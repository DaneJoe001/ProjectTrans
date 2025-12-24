/**
 * @file single_interval.hpp
 * @brief 单区间
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 */
#pragma once

#include <cstdint>
#include <format>

#include "danejoe/stringify/stringify_to_string.hpp"
#include "danejoe/condition/endpoint_range.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe
  */
namespace DaneJoe
{
    /**
     * @brief 区间状态
     */
    enum class IntervalState
    {
        /// @brief 空区间
        Empty,
        /// @brief 全区间
        Whole,
        /// @brief 左固定区间
        FixedLeft,
        /// @brief 右固定区间
        FixedRight,
        /// @brief 左右固定区间
        FixedBoth,
        /// @brief 未知状态
        Unknown
    };
    /**
     * @brief 转换为字符串
     * @param state 区间状态
     * @return std::string 字符串
     */
    std::string to_string(IntervalState state);
    /**
     * @brief 区间边界
     * @tparam T 区间元素类型
     * @note 不含边界方向
     */
    template <typename T>
    struct IntervalBoundary
    {
        /// @brief 区间值
        std::optional<T> m_value;
        /// @brief 是否为开区间
        bool m_is_open;
    };
    /**
     * @brief 区间类
     * @tparam T 区间元素类型
     * @note 排序规则：左端点越小越靠前，左端点相同时右端点越小越靠前
     */
    template <typename T>
    class SingleInterval
    {
    public:
        /**
         * @brief 默认构造函数为空集
         * @note 默认构造区间为(0,0)
         */
        SingleInterval() :m_left(EndpointRange<T>(0, true, BoundarySide::Left)), m_right(EndpointRange<T>(0, true, BoundarySide::Right))
        {
            init_state();
        }
        /**
         * @brief 端点构造函数（单方向无穷区间）
         * @param point 端点
         */
        SingleInterval(EndpointRange<T> point)
            :m_left(point.get_side() == BoundarySide::Left ? point : EndpointRange<T>(BoundarySide::Left)),
            m_right(point.get_side() == BoundarySide::Right ? point : EndpointRange<T>(BoundarySide::Right))
        {
            init_state();
        }
        /**
         * @brief 左右端点构造函数
         * @param left 左端点(不包含方向信息)
         * @param right 右端点(不包含方向信息)
         * @note 传入参数要求左端点值<= 右端点值,否会交换
         */
        SingleInterval(IntervalBoundary<T> left, IntervalBoundary<T> right) :
            m_left(EndpointRange<T>(left.m_value, left.m_is_open, BoundarySide::Left)),
            m_right(EndpointRange<T>(right.m_value, right.m_is_open, BoundarySide::Right))
        {
            if (!left.m_value.has_value() || !right.m_value.has_value()) {}
            else
            {
                if (left.m_value > right.m_value)
                {
                    std::swap(left, right);
                    m_left = EndpointRange<T>(left.m_value, left.m_is_open, BoundarySide::Left);
                    m_right = EndpointRange<T>(right.m_value, right.m_is_open, BoundarySide::Right);
                }
            }
            init_state();
        }
        /**
         * @brief 单点构造函数
         * @param point 单点值
         */
        SingleInterval(const T& point) :m_left({ point,false,BoundarySide::Left }), m_right({ point,false,BoundarySide::Right })
        {
            init_state();
        }
        /**
         * @brief 初始化区间状态
         */
        void init_state()
        {
            if (!m_left.is_limited() && !m_right.is_limited())
            {
                m_state = IntervalState::Whole;
            }
            else if (!m_left.is_limited() && m_right.is_limited())
            {
                m_state = IntervalState::FixedRight;
            }
            else if (m_left.is_limited() && !m_right.is_limited())
            {
                m_state = IntervalState::FixedLeft;
            }
            else if (m_left.is_limited() && m_right.is_limited())
            {
                if (m_left.get_value().value() == m_right.get_value().value() && m_left.is_open() && m_right.is_open())
                {
                    m_state = IntervalState::Empty;
                }
                else
                {
                    m_state = IntervalState::FixedBoth;
                }
            }
        }
        /**
         * @brief 等于运算符
         * @param other 其他区间
         * @return true 等于
         * @return false 不等于
         */
        bool operator==(const SingleInterval& other) const
        {
            return is_equals(other);
        }
        /**
         * @brief 不等于运算符
         * @param other 其他区间
         * @return true 不等于
         * @return false 等于
         */
        bool operator!=(const SingleInterval& other) const
        {
            return !is_equals(other);
        }
        /**
         * @brief 小于运算符
         * @param other 其他区间
         * @return true 小于
         * @return false 大于或等于
         */
        bool operator<(const SingleInterval& other) const
        {
            if (m_left < other.m_left)
            {
                return true;
            }
            else if (other.m_left < m_left)
            {
                return false;
            }
            if (m_right < other.m_right)
            {
                return true;
            }
            return false;
        }
        /**
         * @brief 大于运算符
         * @param other 其他区间
         * @return true 大于
         * @return false 小于或等于
         */
        bool operator>(const SingleInterval& other) const
        {
            return !(*this < other) && !(*this == other);
        }
        /**
         * @brief 等于运算符
         * @param other 其他区间
         * @return true 等于
         * @return false 不等于
         */
        bool is_equals(const SingleInterval& other)const
        {
            return m_left == other.m_left && m_right == other.m_right;
        }
        /**
         * @brief 是否为空区间
         * @return true 空区间
         * @return false 非空区间
         */
        bool is_empty() const
        {
            return m_state == IntervalState::Empty;
        }
        /**
         * @brief 判断值是否在区间内
         * @param value 值
         * @return true 在区间内
         * @return false 不在区间内
         */
        bool is_in_range(T value) const
        {
            return m_left.is_in_range(value) && m_right.is_in_range(value);
        }
        /**
         * @brief 判断区间是否在区间内
         * @param other 其他区间
         * @return true 在区间内
         * @return false 不在区间内
         */
        bool is_in_range(const SingleInterval<T>& other) const
        {
            if (m_left <= other.m_left && other.m_right <= m_right)
            {
                return true;
            }
            return false;
        }
        /**
         * @brief 判断区间是否相邻
         * @param other 其他区间
         * @return true 相邻
         * @return false 不相邻
         */
        bool is_adjacent(const SingleInterval<T>& other) const
        {
            auto self_left_adjacent = m_left.get_adjacent_endpoint();
            auto self_right_adjacent = m_right.get_adjacent_endpoint();
            if (self_left_adjacent == other.m_right ||
                self_right_adjacent == other.m_left)
            {
                return true;
            }
            return false;
        }
        /**
         * @brief 判断区间是否不相交
         * @param other 其他区间
         * @return true 不相交
         * @return false 相交
         */
        bool is_disjoint(const SingleInterval<T>& other) const
        {
            if (is_adjacent(other))
            {
                return false;
            }
            if (m_left > other.m_right)
            {
                if (m_left.get_value() == other.m_right.get_value())
                {
                    return m_left.is_open() && other.m_right.is_open();
                }
                return true;
            }
            else if (m_right < other.m_left)
            {
                if (m_right.get_value() == other.m_left.get_value())
                {
                    return m_right.is_open() && other.m_left.is_open();
                }
                return true;
            }
            return false;
            // return !can_merge_with(other);
        }
        /**
         * @brief 判断区间是否相交
         * @param other 其他区间
         * @return true 相交
         * @return false 不相交
         */
        bool is_intersect(const SingleInterval<T>& other) const
        {
            // 不分离也不相邻即相交
            return !is_disjoint(other) && !is_adjacent(other);
        }

        /**
         * @brief 判断区间是否为单点
         * @return true 单点
         * @return false 非单点
         */
        bool is_point() const
        {
            return m_left.is_limited() && m_right.is_limited() && m_left.get_value() == m_right.get_value() && m_left.is_open() == m_right.is_open() && !m_left.is_open();
        }
        /**
         * @brief 判断区间是否可以合并
         * @param other 其他区间
         * @return true 可以合并
         * @return false 不可以合并
         */
        bool can_merge_with(const SingleInterval<T>& other) const
        {
            return !is_disjoint(other);
        }
        /**
         * @brief 获取区间差集
         * @param other 其他区间
         * @return std::vector<SingleInterval<T>> 差集
         * @note 差集为空时，返回单个空区间
         */
        std::vector<SingleInterval<T>>
            get_difference(const SingleInterval<T>& other) const
        {
            std::vector<SingleInterval<T>> result;
            if (!is_intersect(other))
            {
                result.push_back(*this);
                return result;
            }
            if (other.is_in_range(*this))
            {
                result.push_back(SingleInterval<T>());
                return result;
            }
            auto intersect_opt = try_get_intersect(other);
            if (!intersect_opt.has_value())
            {
                result.push_back(SingleInterval<T>());
                return result;
            }
            auto intersect = intersect_opt.value();

            auto intersect_left_adjacent =
                intersect.m_left.get_adjacent_endpoint();
            auto intersect_right_adjacent =
                intersect.m_right.get_adjacent_endpoint();
            // 区分区间情况
            if (intersect.m_left > m_left && intersect.m_right < m_right)
            {

                result.push_back(
                    SingleInterval<T>({ m_left.get_value(), m_left.is_open() },
                    { intersect_left_adjacent.get_value(),
                     intersect_left_adjacent.is_open() }));
                result.push_back(
                    SingleInterval<T>({ intersect_right_adjacent.get_value(),intersect_right_adjacent.is_open() },
                    { m_right.get_value(), m_right.is_open() }));
            }
            else if (intersect.m_left == m_left)
            {
                result.push_back(
                    SingleInterval<T>({ intersect_right_adjacent.get_value(), intersect_right_adjacent.is_open() }, { m_right.get_value(),m_right.is_open() }));
            }
            else if (intersect.m_right == m_right)
            {
                result.push_back(
                    SingleInterval<T>({ m_left.get_value(), m_left.is_open() },
                    { intersect_left_adjacent.get_value(),
                     intersect_left_adjacent.is_open() }));
            }
            return result;
        }

        /**
         * @brief 尝试获取区间交集
         * @param other 其他区间
         * @return std::optional<SingleInterval<T>> 交集
         */
        std::optional<SingleInterval<T>> try_get_intersect(const SingleInterval<T>& other) const
        {
            if (!is_intersect(other))
            {
                return std::nullopt;
            };
            auto left = m_left <= other.m_left ? other.m_left : m_left;
            auto right = m_right >= other.m_right ? other.m_right : m_right;
            return SingleInterval<T>({ left.get_value(),left.is_open() }, { right.get_value(),right.is_open() });
        }
        /**
         * @brief 尝试获取区间并集
         * @param other 其他区间
         * @return std::optional<SingleInterval<T>> 并集
         */
        std::optional<SingleInterval<T>> try_get_union(const SingleInterval<T>& other) const
        {
            if (!can_merge_with(other))
            {
                return std::nullopt;
            }
            auto left = m_left <= other.m_left ? m_left : other.m_left;
            auto right = m_right >= other.m_right ? m_right : other.m_right;
            return SingleInterval<T>({ left.get_value(),left.is_open() }, { right.get_value(),right.is_open() });
        }
        /**
         * @brief 尝试通过交集合并区间
         * @param other 其他区间
         * @return true 合并成功
         * @return false 合并失败
         */
        bool try_merge_by_intersection(const SingleInterval<T>& other)
        {
            auto intersect = try_get_intersect(other);
            if (!intersect.has_value())
            {
                return false;
            }
            *this = intersect.value();
            return true;
        }
        /**
         * @brief 尝试通过并集合并区间
         * @param other 其他区间
         * @return true 合并成功
         * @return false 合并失败
         */
        bool try_merge_by_union(const SingleInterval<T>& other)
        {
            auto union_ = try_get_union(other);
            if (!union_.has_value())
            {
                return false;
            }
            *this = union_.value();
            return true;
        }
        /**
         * @brief 转换为字符串
         * @return std::string 字符串
         */
        std::string to_string() const
        {
            std::string left_sign = m_left.is_open() ? "(" : "[";
            std::string right_sign = m_right.is_open() ? ")" : "]";
            std::string left_value = m_left.get_value().has_value() ? DaneJoe::to_string(m_left.get_value().value()) : "-inf";
            std::string right_value = m_right.get_value().has_value() ? DaneJoe::to_string(m_right.get_value().value()) : "+inf";
            return std::format(
                "state={} | is_empty={} | is_point={} | left_sign={} | left_value={} | right_sign={} | right_value={}",
                DaneJoe::to_string(m_state),
                m_state == IntervalState::Empty,
                is_point(),
                left_sign,
                left_value,
                right_sign,
                right_value);
        }
        std::optional<SingleInterval<int64_t>> get_integer_interval();
        EndpointRange<T> get_left_endpoint() const
        {
            return m_left;
        }
        EndpointRange<T> get_right_endpoint() const
        {
            return m_right;
        }
    private:
        /// @brief 区间状态
        IntervalState m_state = IntervalState::Empty;
        /// @brief 左端点
        EndpointRange<T> m_left;
        /// @brief 右端点
        EndpointRange<T> m_right;
    };
    template<typename T>
    std::optional<SingleInterval<int64_t>> SingleInterval<T>::get_integer_interval()
    {
        if constexpr (!std::is_integral_v<T>)
        {
            return std::nullopt;
        }
        else
        {
            auto left_ep_opt = m_left.get_integer_endpoint();
            auto right_ep_opt = m_right.get_integer_endpoint();
            SingleInterval<int64_t> result(
                { left_ep_opt->get_value(),left_ep_opt->is_open() }, { right_ep_opt->get_value(),right_ep_opt->is_open() });
            return result;
        }
    }
}

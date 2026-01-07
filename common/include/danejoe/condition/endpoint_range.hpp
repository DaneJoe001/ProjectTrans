/**
 * @file endpoint_range.hpp
 * @brief 端点范围
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-11-30
 * @details 定义用于表达半无限区间端点的 EndpointRange<T>。
 *          端点值使用 std::optional<T> 表示：
 *          - 有值：有限端点
 *          - std::nullopt：无穷端点（-∞ 或 +∞，由 BoundarySide 决定）
 *          该类型可用于构造区间约束，并提供端点排序、相邻端点判断与成员值过滤等工具方法。
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/common/enum/enum_convert.hpp"
#include "danejoe/stringify/stringify_to_string.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 边界侧
     * @details 用于表示端点在数轴上的方向：
     *          - Left：左端点（形如 [a,+∞) 或 (-∞,+∞) 的左侧）
     *          - Right：右端点（形如 (-∞,b] 或 (-∞,+∞) 的右侧）
     *          - Unknown：未知/未指定
     */
    enum class BoundarySide
    {
        /// @brief 左边界
        Left,
        /// @brief 右边界
        Right,
        /// @brief 未知边界
        Unknown
    };
    /**
     * @brief 边界侧转换为字符串
     * @param side 边界侧
     * @return 对应的枚举字符串
     */
    std::string to_string(BoundarySide side);
    /**
     * @brief 边界侧转换为枚举
     * @param enum_string 枚举字符串
     * @return 对应的枚举值，未知时返回 BoundarySide::Unknown
     */
    template<>
    BoundarySide enum_cast<BoundarySide>(const std::string& enum_string);
    /**
     * @brief 端点结构
     * @note 排序规则遵循坐标轴规范(-∞,+∞)
     * @example (-inf<x)<(x<a)<(x<=a)<(x>=a)<(x>a)
     * @note 当端点值皆可取到且相等时，右端点<左端点
     * @tparam T 端点值类型
     * @details 该类型用于描述“单侧约束端点”，并可作为区间左右端点组合使用。
     *          - m_value 为 std::nullopt 表示无穷端点
     *          - m_is_open 表示端点是否为开端点（不包含该值）
     *          - m_side 表示该端点作为左端点或右端点
     */
    template<typename T>
    class EndpointRange
    {
    public:
        /**
         * @brief 拷贝构造函数
         * @param other 其他端点范围
         */
        EndpointRange(const EndpointRange& other)
        {
            m_value = other.m_value;
            m_is_open = other.m_is_open;
            m_side = other.m_side;
        }
        /**
         * @brief 构造函数
         * @param side 端点方向（Left 表示 -∞，Right 表示 +∞）
         * @note 仅传入方向选项时构造无穷端点；无穷端点始终为开端点。
         */
        EndpointRange(BoundarySide side) :
            m_value(std::nullopt),
            m_is_open(true),
            m_side(side)
        {}
        /**
         * @brief 构造函数
         * @param value 端点值
         * @param is_open 是否为开区间
         * @param side 端点方向
         * @note 若 value 为 std::nullopt（无穷端点），则 is_open 会自动设置为 true。
         */
        EndpointRange(
            std::optional<T> value,
            bool is_open,
            BoundarySide side) :
            m_value(value),
            m_is_open(is_open),
            m_side(side)
        {
            if (!m_value.has_value())
            {
                m_is_open = true;
            }
        }
        /**
         * @brief 等于运算符
         * @param other 其他端点范围
         * @return true 等于
         * @return false 不等于
         */
        bool operator==(const EndpointRange<T>& other) const
        {
            return m_value == other.m_value && m_is_open == other.m_is_open && m_side == other.m_side;
        }
        /**
         * @brief 不等于运算符
         * @param other 其他端点范围
         * @return true 不等于
         * @return false 等于
         */
        bool operator!=(const EndpointRange<T>& other) const
        {
            return m_value != other.m_value || m_is_open != other.m_is_open || m_side != other.m_side;
        }
        /**
         * @brief 小于运算符
         * @param other 其他端点范围
         * @return true 小于
         * @return false 大于或等于
         */
        bool operator<(const EndpointRange<T>& other) const
        {
            // 相等时直接返回false
            if (*this == other)
            {
                return false;
            }
            // 都为无穷端点
            if (!this->m_value.has_value() && !other.m_value.has_value())
            {
                // 仅有自身在左的时候认为自身更小
                return this->m_side == BoundarySide::Left && other.m_side == BoundarySide::Right;
            }
            // 皆为有限端点
            else if (this->m_value.has_value() && other.m_value.has_value())
            {
                if (this->m_value.value() != other.m_value.value())
                {
                    return this->m_value.value() < other.m_value.value();
                }
                else
                {
                    if (this->m_side == other.m_side)
                    {
                        if (this->m_side == BoundarySide::Left)
                        {
                            return !this->m_is_open;
                        }
                        else
                        {
                            return this->m_is_open;
                        }
                    }
                    else
                    {
                        return this->m_side == BoundarySide::Right;
                    }
                }
            }
            // 一个无穷端点
            else
            {
                // 两个端点在同侧
                if (this->m_side == other.m_side)
                {
                    if (this->m_side == BoundarySide::Left)
                    {
                        // 左侧为无穷区间时最小
                        return !this->m_value.has_value();
                    }
                    else
                    {
                        // 右侧为无穷区间时最大
                        return this->m_value.has_value();
                    }
                }
                else
                {
                    // 无穷或有限端点只要在左侧皆小
                    return this->m_side == BoundarySide::Left;
                }
            }
        }
        /**
         * @brief 大于运算符
         * @param other 其他端点范围
         * @return true 大于
         * @return false 小于或等于
         */
        bool operator>(const EndpointRange<T>& other) const
        {
            return other < *this;
        }
        /**
         * @brief 小于等于运算符
         * @param other 其他端点范围
         * @return true 小于等于
         * @return false 大于
         */
        bool operator<=(const EndpointRange<T>& other) const
        {
            return *this < other || *this == other;
        }
        /**
         * @brief 大于等于运算符
         * @param other 其他端点范围
         * @return true 大于等于
         * @return false 小于
         */
        bool operator>=(const EndpointRange<T>& other) const
        {
            return *this > other || *this == other;
        }
        /**
         * @brief 判断值是否在范围内
         * @param value 值
         * @return true 在范围内
         * @return false 不在范围内
         * @details 该端点描述的是“单侧约束”。
         *          - Right 端点：表示 value 小于该端点，或等于且为闭端点
         *          - Left 端点：表示 value 大于该端点，或等于且为闭端点
         *          无穷端点对任意 value 恒为 true。
         */
        bool is_in_range(const T& value) const
        {
            // 无界端点
            if (!m_value.has_value())
            {
                return true;
            }
            //有界且极限右端点(-∞,Right]
            if (m_side == BoundarySide::Right)
            {
                // 目标值在端点值左边
                if (value < m_value.value())
                {
                    return true;
                }
                // 目标值在端点值右边
                else if (value > m_value.value())
                {
                    return false;
                }
                // 目标值等于端点值
                else
                {
                    return !m_is_open;
                }
            }
            //有界且极限左端点[Left,+∞)
            else
            {
                // 端点值在目标值左边
                if (value < m_value.value())
                {
                    return false;
                }
                // 端点值在目标值右边
                else if (value > m_value.value())
                {
                    return true;
                }
                // 端点值等于目标值
                else
                {
                    return !m_is_open;
                }
            }
        }
        /**
         * @brief 是否为开区间
         * @return true 开区间
         * @return false 闭区间
         */
        bool is_open() const
        {
            return m_is_open;
        }
        /**
         * @brief 是否为闭区间
         * @return true 闭区间
         * @return false 开区间
         */
        bool is_closed() const
        {
            return !m_is_open;
        }
        /**
         * @brief 是否为有限区间
         * @return true 有限区间
         * @return false 无穷区间
         */
        bool is_limited() const
        {
            return m_value.has_value();
        }
        /**
         * @brief 是否为左有限区间
         * @return true 左有限区间
         * @return false 右有限区间
         */
        bool is_left_limited() const
        {
            return m_side == BoundarySide::Left;
        }
        /**
         * @brief 是否为右有限区间
         * @return true 右有限区间
         * @return false 左有限区间
         */
        bool is_right_limited() const
        {
            return m_side == BoundarySide::Right;
        }
        /**
         * @brief 获取端点值
         * @return std::optional<T> 端点值
         */
        std::optional<T> get_value() const
        {
            return m_value;
        }
        /**
         * @brief 获取端点侧
         * @return BoundarySide 端点侧
         */
        BoundarySide get_side()const
        {
            return m_side;
        }
        /**
         * @brief 获取相邻的端点
         * @return EndpointRange<T>
         * @details 相邻端点定义为：端点值相同、开闭性互补、side 相反。
         */
        EndpointRange<T> get_adjacent_endpoint() const
        {
            auto new_side = m_side == BoundarySide::Left ? BoundarySide::Right : BoundarySide::Left;
            return EndpointRange<T>(m_value, !m_is_open, new_side);
        }
        /**
         * @brief 是否与另一个端点范围相邻
         * @param other 另一个端点范围
         * @return true 相邻
         * @return false 不相邻
         * @details 若 other 等于当前端点的 get_adjacent_endpoint()，则视为相邻。
         */
        bool is_adjacent_with(const EndpointRange<T>& other) const
        {
            auto adjacent_point = get_adjacent_endpoint();
            return adjacent_point == other;
        }
        /**
         * @brief 获取范围内的值
         * @param values 值列表
         * @return std::vector<T> 范围内的值
         */
        std::vector<T> get_values_in_range(const std::vector<T>& values)
        {
            std::vector<T> result;
            for (const auto& value : values)
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
        std::vector<T> get_values_not_in_range(const std::vector<T>& values)
        {
            std::vector<T> result;
            for (const auto& value : values)
            {
                if (!is_in_range(value))
                {
                    result.push_back(value);
                }
            }
            return result;
        }
        /**
         * @brief 转换为字符串
         * @param variable_name 变量名
         * @return std::string 字符串
         * @details 将端点表达为形如 "(x < a)"、"(a <= x)"、"(-∞ < x)" 的字符串形式。
         */
        std::string to_string(const std::string& variable_name = "x") const
        {
            std::string result = "(";
            std::string value;
            if (m_value.has_value())
            {
                value = DaneJoe::to_string(m_value.value());
            }
            else
            {
                value = m_side == BoundarySide::Left ? "-∞" : "+∞";
            }
            std::string expression;
            SignDirection direction = SignDirection::Unknown;
            if (m_side == BoundarySide::Left)
            {
                if (direction == SignDirection::SmallToLarge || direction == SignDirection::VarRight)
                {
                    expression = m_is_open ?
                        value + " < " + variable_name :
                        value + " <= " + variable_name;
                }
                else if (direction == SignDirection::LargeToSmall || direction == SignDirection::VarLeft)
                {
                    expression = m_is_open ?
                        variable_name + " > " + value :
                        variable_name + " >= " + value;
                }
                else
                {
                    expression = m_is_open ?
                        value + " < " + variable_name :
                        value + " <= " + variable_name;
                }
            }
            else
            {
                if (direction == SignDirection::SmallToLarge || direction == SignDirection::VarLeft)
                {
                    expression = m_is_open ?
                        variable_name + " < " + value :
                        variable_name + " <= " + value;
                }
                else if (direction == SignDirection::LargeToSmall || direction == SignDirection::VarRight)
                {
                    expression = m_is_open ?
                        value + " > " + variable_name :
                        value + " >= " + variable_name;
                }
                else
                {
                    expression = m_is_open ?
                        variable_name + " < " + value :
                        variable_name + " <= " + value;
                }
            }
            result += expression;
            result += ")";
            return result;
        }
        /**
         * @brief 获取整数端点
         * @return 若 T 为整数类型则返回对应的 EndpointRange<int64_t>；否则返回 std::nullopt
         * @details 用于将整数端点统一提升为 int64_t 表示，便于后续统一处理。
         */
        std::optional<EndpointRange<int64_t>> get_integer_endpoint() const;
    private:
        /// @brief 端点值,std::nullopt代表无穷
        std::optional<T> m_value;
        /// @brief 是否为开区间端点
        bool m_is_open;
        /// @brief 端点侧(相对于可选值的方向)
        /// @note 相当于区间左值(left...value...right)
        BoundarySide m_side;
    };
    /**
     * @brief 判断值是否位于两个端点之间
     * @tparam T 值类型
     * @param left_endpoint 左端点(无严格大小关系)
     * @param right_endpoint 右端点(无严格大小关系)
     * @param value 值
     * @return true 值位于两个端点之间
     * @return false 值不位于两个端点之间
     * @details 该函数等价于同时满足：
     *          - value 在 left_endpoint 描述的单侧约束内
     *          - value 在 right_endpoint 描述的单侧约束内
     */
    template<typename T>
    bool is_in_range(
        const EndpointRange<T>& left_endpoint,
        const EndpointRange<T>& right_endpoint,
        const T& value)
    {
        return left_endpoint.is_in_range(value) && right_endpoint.is_in_range(value);
    }
    /**
     * @copydoc EndpointRange::get_integer_endpoint
     */
    template<typename T>
    std::optional<EndpointRange<int64_t>> EndpointRange<T>::get_integer_endpoint() const
    {
        if constexpr (!std::is_integral_v<T>)
        {
            return std::nullopt;
        }
        else
        {
            std::optional<int64_t> value = std::nullopt;
            if (m_value.has_value())
            {
                value = static_cast<int64_t>(m_value.value());
            }
            EndpointRange<int64_t> result(value, m_is_open, m_side);
            return result;
        }
    }
}

/**
 * @file scatter_point_range.hpp
 * @brief 散点范围
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 */
#pragma once

#include <set>
#include <vector>
#include <sstream>

#include "danejoe/stringify/stringify_to_string.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ScatterPointRange
     * @brief 散点范围
     * @tparam T 散点类型
     */
    template<typename T>
    class ScatterPointRange
    {
    public:
        /**
         * @brief 默认构造函数
         */
        ScatterPointRange() {}
        /**
         * @brief 构造函数
         * @param value 散点值
         */
        ScatterPointRange(const T& value)
        {
            m_scatter_elements.insert(value);
        }
        /**
         * @brief 构造函数
         * @param values 散点值列表
         */
        ScatterPointRange(const std::vector<T>& values)
        {
            m_scatter_elements.insert(values.begin(), values.end());
        }
        /**
         * @brief 构造函数
         * @param other 其他散点范围
         */
        ScatterPointRange(const std::set<T>& other)
        {
            m_scatter_elements = other;
        }
        /**
         * @brief 判断值是否在范围内
         * @param value 值
         * @return true 在范围内
         * @return false 不在范围内
         */
        bool is_in_range(const T& value)const
        {
            return m_scatter_elements.find(value) != m_scatter_elements.end();
        }
        /**
         * @brief 获取散点元素
         * @return std::vector<T> 散点元素
         */
        std::vector<T> get_scatter_elements()const
        {
            std::vector<T> result = std::vector<T>(m_scatter_elements.begin(), m_scatter_elements.end());
            return result;
        }
        /**
         * @brief 添加散点元素
         * @param element 散点元素
         */
        void add_scatter_element(const T& element)
        {
            m_scatter_elements.insert(element);
        }
        /**
         * @brief 添加批量的元素
         * @param elements 散点元素列表
         * @todo 扩展到可迭代容器模板
         */
        void add_scatter_elements(const std::vector<T>& elements)
        {
            m_scatter_elements.insert(elements.begin(), elements.end());
        }
        /**
         * @brief 添加散点范围
         * @param scatter_point 散点范围
         */
        void add_scatter_elements(const ScatterPointRange<T>& scatter_point)
        {
            m_scatter_elements.insert(scatter_point.m_scatter_elements.begin(), scatter_point.m_scatter_elements.end());
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
         * @brief 删除散点元素
         * @param element 散点元素
         */
        void remove_scatter_element(const T& element)
        {
            m_scatter_elements.erase(element);
        }
        /**
         * @brief 转换为字符串
         * @param variable_name 变量名
         * @return std::string 字符串
         */
        std::string to_string(const std::string& variable_name = "x")const
        {
            std::stringstream ss;
            ss << variable_name << ": {";
            for (const auto& value : m_scatter_elements)
            {
                ss << DaneJoe::to_string(value);
                if (&value != &(*m_scatter_elements.rbegin()))
                {
                    ss << ", ";
                }
            }
            ss << "}";
            return ss.str();
        }
    private:
        /// @brief 散点元素
        std::set<T> m_scatter_elements;
    };
}

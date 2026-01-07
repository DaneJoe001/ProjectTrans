/**
 * @file byte_order.hpp
 * @brief 字节序处理工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 */
#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum ByteOrder
     * @brief  字节序
     */
    enum class ByteOrder :uint8_t
    {
        /// @brief 未知字节序
        Unknown,
        /// @brief 小端字节序
        LittleEndian,
        /// @brief 大端字节序
        BigEndian
    };
    /**
     * @brief 获取字节序枚举字符串（调试用）
     * @param order 字节序
     * @return 对应的枚举字符串
     */
    std::string to_string(ByteOrder order);
    /**
     * @brief 从字符串转换为字节序
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 ByteOrder::Unknown
     */
    template<>
    ByteOrder enum_cast<ByteOrder>(const std::string& enum_string);
    /// @brief 默认网络字节序为大端（BigEndian）
    const ByteOrder DANEJOE_NETWORK_BYTE_ORDER = ByteOrder::BigEndian;
    /**
     * @brief 获取主机字节序
     * @return 主机字节序
     */
    ByteOrder get_local_byte_order()noexcept;
    /**
     * @brief 判断主机字节序和网络字节序是否一致
     * @note 默认网络字节序为大端
     * @return true 主机字节序与网络字节序一致
     * @return false 主机字节序与网络字节序不一致
     */
    bool is_network_byte_order();
    /**
     * @brief 转换字节序（指针版本）
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     * @note 仅对对象表示（object representation）进行字节翻转；不处理高层语义。
     * @note dest 与 data 均须指向至少 sizeof(T) * count 字节的有效内存。
     * @note 如果 dest 为 nullptr 或 data 为 nullptr，则不进行转换。
     * @note 如果 is_network_byte_order() 为 true，则直接 memcpy（无翻转）。
     * @note dest 与 data 内存区域应避免重叠（否则 memcpy 行为未定义）。
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void convert_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        if (dest == nullptr || data == nullptr)
        {
            return;
        }
        if (is_network_byte_order())
        {
            std::memcpy(dest, data, sizeof(T) * count);
        }
        else
        {
            for (uint32_t i = 0; i < count; i++)
            {
                // 每个元素目标的开始位置
                uint32_t dest_index = i * sizeof(T);
                // 每个元素源的结束位置
                const uint8_t* src_ptr = reinterpret_cast<const uint8_t*>(data + i);
                for (uint32_t j = 0; j < sizeof(T); j++)
                {
                    dest[dest_index + j] = src_ptr[sizeof(T) - j - 1];
                }
            }
        }
    }
    /**
     * @brief 转换字节序（常引用版本）
     * @tparam T 数据类型（不包含常引用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     * @note 单字节不需要转换，但是可以写入指定位置
     * @note dest 须指向至少 sizeof(T) 字节的有效内存。
     * @note 如果 dest 为 nullptr，则不进行转换。
     * @note 如果 is_network_byte_order() 为 true，则直接 memcpy（无翻转）。
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void convert_byte_order(uint8_t* dest, const T& data)
    {
        if (dest == nullptr)
        {
            return;
        }
        if (is_network_byte_order())
        {
            std::memcpy(dest, &data, sizeof(T));
        }
        else
        {
            uint32_t dest_index = 0;
            uint32_t src_index = sizeof(T) - 1;
            uint8_t* dest_ptr = dest;
            const uint8_t* src_ptr = reinterpret_cast<const uint8_t*>(&data);
            for (;dest_index < sizeof(T);dest_index++, src_index--)
            {
                *(dest_ptr + dest_index) = *(src_ptr + src_index);
            }
        }
    }
    /**
     * @brief 将给定数量的字节翻转至网络字节序
     * @note 字节序转换仅针对字节序列本身；具体类型序列化可配合 to_byte_array 使用。
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @note 后续再考虑分标量模板和连续序列
     * @param data 字节流数据
     * @param count 元素数量
     * @return 改变字节序后的字节流数据
     */
    std::vector<uint8_t> to_network_byte_order(const uint8_t* data, uint32_t count);
    /**
     * @brief 将给定数量的字节翻转至主机字节序
     * @param data 字节流数据
     * @param count 元素数量
     * @note 字节序转换仅针对字节序列本身；具体类型序列化可配合 to_byte_array 使用。
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_local_byte_order(const uint8_t* data, uint32_t count);
    /**
     * @brief 将给定的字节流统一调整至网络字节序
     * @param data 字节流数据
     * @note 字节序转换仅针对字节序列本身；具体类型序列化可配合 to_byte_array 使用。
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_network_byte_order(const std::vector<uint8_t>& data);
    /**
     * @brief 将给定的字节流统一调整至主机字节序
     * @param data 字节流数据
     * @note 字节序转换仅针对字节序列本身；具体类型序列化可配合 to_byte_array 使用。
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_local_byte_order(const std::vector<uint8_t>& data);
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含常引用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void to_local_byte_order(uint8_t* dest, const T& data)
    {
        convert_byte_order<T>(dest, data);
    }
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void to_local_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        convert_byte_order<T>(dest, data, count);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含常引用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void to_network_byte_order(uint8_t* dest, const T& data)
    {
        convert_byte_order<T>(dest, data);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        void to_network_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        convert_byte_order<T>(dest, data, count);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含常引用属性）（要求平凡可复制）
     * @param data 数据
     * @return 改变字节序后的数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        T to_network_byte_order_value(const T& data)
    {
        std::vector<uint8_t> dest(sizeof(T));
        convert_byte_order<T>(dest.data(), data);
        T result;
        std::memcpy(&result, dest.data(), sizeof(T));
        return result;
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param data 源起始地址
     * @param count 元素数量
     * @return 改变字节序后的数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        std::vector<T> to_network_byte_order_array(const T* data, uint32_t count = 1)
    {
        std::vector<T> dest(count);
        std::memcpy(dest.data(), data, sizeof(T) * count);
        std::vector<T> result(count);
        for (uint32_t i = 0; i < count; i++)
        {
            convert_byte_order<T>(reinterpret_cast<uint8_t*>(result.data() + i), dest[i]);
        }
        return result;
    }
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含常引用属性）（要求平凡可复制）
     * @param data 数据
     * @return 改变字节序后的数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        T to_local_byte_order_value(const T& data)
    {
        std::vector<uint8_t> dest(sizeof(T));
        convert_byte_order<T>(dest.data(), data);
        T result;
        std::memcpy(&result, dest.data(), sizeof(T));
        return result;
    }
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param data 源起始地址
     * @param count 元素数量
     * @return 改变字节序后的数据
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        std::vector<T> to_local_byte_order_array(const T* data, uint32_t count = 1)
    {
        std::vector<T> dest(count);
        std::memcpy(dest.data(), data, sizeof(T) * count);
        std::vector<T> result(count);
        for (uint32_t i = 0; i < count; i++)
        {
            convert_byte_order<T>(reinterpret_cast<uint8_t*>(result.data() + i), dest[i]);
        }
        return result;
    }
}

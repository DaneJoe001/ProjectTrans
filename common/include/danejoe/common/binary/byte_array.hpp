/**
 * @file byte_array.hpp
 * @brief 字节数组处理工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 */
#pragma once

#include <vector>
#include <cstring>
#include <cstdint>
#include <optional>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 将任意类型转化为字节数组
     * @tparam T 数据类型
     * @param data 数据
     * @return 字节数组
     * @note 不参与字节序转换，结果为当前平台/编译器下的对象表示（object representation）。
     * @note 建议仅用于平凡可复制（trivially copyable）类型；否则可能包含实现相关的内部状态。
     */
    template<class T>
    std::vector<uint8_t> to_byte_array(const T& data)
    {
        std::vector<uint8_t> byte_array(sizeof(T));
        std::memcpy(byte_array.data(), &data, sizeof(T));
        return byte_array;
    }
    /**
     * @brief 将任意类型转化为字节数组
     * @tparam T 数据类型
     * @param ptr 数据指针
     * @param count 元素数量
     * @return 字节数组
     * @note 不参与字节序转换，结果为当前平台/编译器下的对象表示（object representation）。
     * @note 当 ptr 为 nullptr 时返回空数组。
     * @note 建议仅用于平凡可复制（trivially copyable）类型。
     */
    template<class T>
    std::vector<uint8_t> to_byte_array(const T* ptr, uint32_t count = 1)
    {
        if (ptr == nullptr)
        {
            return std::vector<uint8_t>();
        }
        std::vector<uint8_t> byte_array(sizeof(T) * count);
        std::memcpy(byte_array.data(), ptr, sizeof(T) * count);
        return byte_array;
    }
    /**
     * @brief 将字节数组转换为指定类型
     * @tparam T 目标数据类型（要求平凡可复制）
     * @param bytes 字节数组
     * @return 转换后的数据；当字节数组大小不足以容纳指定类型时返回 std::nullopt
     * @note 本函数不做字节序转换，按当前平台字节序解释 bytes。
     * @warning 当前实现通过 reinterpret_cast 直接读取 bytes.data()。
     *          若 bytes.data() 未满足 T 的对齐要求，或在严格别名规则下，可能触发未定义行为。
     *          若需要更强可移植性，建议改用 std::memcpy 到局部变量再返回。
     */
    template <class T, typename =
        std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
        std::optional<T> try_cast(const std::vector<uint8_t>& bytes)
    {
        if (sizeof(T) > bytes.size())
        {
            return std::nullopt;
        }
        return *reinterpret_cast<const T*>(bytes.data());
    }
}

#pragma once

#include <vector>
#include <cstring>
#include <cstdint>
#include <optional>

namespace DaneJoe
{
    /**
     * @brief  字节序
     */
    enum class ByteOrder :uint8_t
    {
        Unknown,
        /// @brief 小端字节序
        LittleEndian,
        /// @brief 大端字节序
        BigEndian
    };
    /**
     * @brief 将任意类型转化为字节数组
     * @note 不参与字节序转换
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
     * @param ptr 数据指针
     * @param count 元素数量
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
    /// @brief 默认网络字节序为大端
    const ByteOrder g_network_byte_order = ByteOrder::BigEndian;
    /**
     * @brief 获取主机字节序
     */
    ByteOrder get_local_byte_order()noexcept;
    /**
     * @brief 判断主机字节序和网络字节序是否一致
     * @note 默认网络字节序为大端
     */
    bool is_local_byte_order_same_to_network();
    /**
     * @brief 转换字节序（指针版本）
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void turn_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        if (dest == nullptr || data == nullptr)
        {
            return;
        }
        if (is_local_byte_order_same_to_network())
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
     * @brief 转换字节序（常饮用版本）
     * @tparam T 数据类型（不包含常饮用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     * @note 单字节不需要转换，但是可以写入指定位置
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void turn_byte_order(uint8_t* dest, const T& data)
    {
        if (dest == nullptr)
        {
            return;
        }
        if (is_local_byte_order_same_to_network())
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
     * @param data 字节流数据
     * @note 具体类型通过get_byte_array转换
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_network_byte_order(const uint8_t* data, uint32_t count);
    /**
     * @brief 将给定数量的字节翻转至主机字节序
     * @param data 字节流数据
     * @note 具体类型通过get_byte_array转换
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_local_byte_order(const uint8_t* data, uint32_t count);
    /**
     * @brief 将给定的字节流统一调整至网络字节序
     * @param data 字节流数据
     * @note 具体类型通过get_byte_array转换
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_network_byte_order(const std::vector<uint8_t>& data);
    /**
     * @brief 将给定的字节流统一调整至主机字节序
     * @param data 字节流数据
     * @note 具体类型通过get_byte_array转换
     * @note 避免对当前平台数据产生影响，故内部使用拷贝转换
     * @return 改变字节序后的字节流数据
     * @note 后续再考虑分标量模板和连续序列
     */
    std::vector<uint8_t> to_local_byte_order(const std::vector<uint8_t>& data);
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含常饮用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void to_local_byte_order(uint8_t* dest, const T& data)
    {
        turn_byte_order<T>(dest, data);
    }
    /**
     * @brief 转换字节序至主机字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void to_local_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        turn_byte_order<T>(dest, data, count);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含常饮用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void to_network_byte_order(uint8_t* dest, const T& data)
    {
        turn_byte_order<T>(dest, data);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    void to_network_byte_order(uint8_t* dest, const T* data, uint32_t count = 1)
    {
        turn_byte_order<T>(dest, data, count);
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含常饮用属性）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    T to_network_byte_order_value(const T& data)
    {
        std::vector<uint8_t> dest(sizeof(T));
        turn_byte_order<T>(dest.data(), data);
        T result;
        std::memcpy(&result, dest.data(), sizeof(T));
        return result;
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    std::vector<T> to_network_byte_order_array(const T* data, uint32_t count = 1)
    {
        std::vector<T> dest(count);
        std::memcpy(dest.data(), data, sizeof(T) * count);
        std::vector<T> result(count);
        for (uint32_t i = 0; i < count; i++)
        {
            turn_byte_order<T>(reinterpret_cast<uint8_t*>(result.data() + i), dest[i]);
        }
        return result;
    }
    /**
 * @brief 转换字节序至网络字节序
 * @tparam T 数据类型（不包含常饮用属性）（要求平凡可复制）
 * @param dest 目标地址
 * @param data 数据
 */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    T to_local_byte_order_value(const T& data)
    {
        std::vector<uint8_t> dest(sizeof(T));
        turn_byte_order<T>(dest.data(), data);
        T result;
        std::memcpy(&result, dest.data(), sizeof(T));
        return result;
    }
    /**
     * @brief 转换字节序至网络字节序
     * @tparam T 数据类型（不包含指针）（要求平凡可复制）
     * @param dest 目标地址
     * @param data 源起始地址
     * @param count 元素数量
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    std::vector<T> to_local_byte_order_array(const T* data, uint32_t count = 1)
    {
        std::vector<T> dest(count);
        std::memcpy(dest.data(), data, sizeof(T) * count);
        std::vector<T> result(count);
        for (uint32_t i = 0; i < count; i++)
        {
            turn_byte_order<T>(reinterpret_cast<uint8_t*>(result.data() + i), dest[i]);
        }
        return result;
    }

    template<typename T>
    bool ensure_enough_capacity(std::vector<T>& data, uint32_t size)
    {
        /// @todo 考虑捕获异常
        if (data.size() >= size)
        {
            return true;
        }
        data.resize(size);
        return true;
    }
    template<typename T>
    bool ensure_enough_to_read(const std::vector<T>& data, uint32_t size)
    {
        /// @todo 考虑捕获异常
        if (data.size() >= size)
        {
            return true;
        }
        return false;
    }
    /**
     * @brief 将字节数组转换为指定类型
     * @note 返回不同类型的数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    std::optional<T> to_value(const std::vector<uint8_t>& bytes)
    {
        if (sizeof(T) > bytes.size())
        {
            return std::nullopt;
        }
        return *reinterpret_cast<const T*>(bytes.data());
    }
}
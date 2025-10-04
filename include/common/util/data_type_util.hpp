#pragma once

#include <map>
#include <any>
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

namespace DaneJoe
{
    /**
     * @enum DataType
     * @brief 数据类型
     * @note 主要是为了反序列化时能返回正确的类型
     */
    enum class DataType :uint8_t
    {
        Object,
        UInt8,
        Int8,
        UInt16,
        Int16,
        UInt32,
        Int32,
        UInt64,
        Int64,
        /// @brief 后续再确定浮点数规范
        Float,
        Double,
        /// @brief 使用一字节01表示
        Bool,
        /// @brief 字符串，字符编码由标志位确定
        /// @note 当前默认使用UTF-8，后续再拓展
        String,
        /// @brief 字节流
        ByteArray,
        Map,
        Array,
        Dictionary,
        Null,
        Unknown
    };

    /**
     * @brief 获取数据类型枚举
     *@tparam 数据类型
     */
    template <class T>
    DataType get_data_type()
    {
        if constexpr (std::is_same_v<T, uint8_t>)
        {
            return DataType::UInt8;
        }
        else if constexpr (std::is_same_v<T, int8_t>)
        {
            return DataType::Int8;
        }
        else if constexpr (std::is_same_v<T, uint16_t>)
        {
            return DataType::UInt16;
        }
        else if constexpr (std::is_same_v<T, int16_t>)
        {
            return DataType::Int16;
        }
        else if constexpr (std::is_same_v<T, uint32_t>)
        {
            return DataType::UInt32;
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            return DataType::Int32;
        }
        else if constexpr (std::is_same_v<T, uint64_t>)
        {
            return DataType::UInt64;
        }
        else if constexpr (std::is_same_v<T, int64_t>)
        {
            return DataType::Int64;
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            return DataType::Float;
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return DataType::Double;
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            return DataType::Bool;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            return DataType::String;
        }
        else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
        {
            return DataType::ByteArray;
        }
        else if constexpr (std::is_same_v<T, std::nullptr_t>)
        {
            return DataType::Null;
        }
        else
        {
            return DataType::Unknown;
        }
    }
    /**
     * @brief 获取数据类型长度
     * @note 仅支持基础数据类型
     */
    uint32_t get_data_type_length(DataType type);

}
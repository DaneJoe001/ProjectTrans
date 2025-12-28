/**
 * @file serialize_map_value.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 映射值序列化
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include "danejoe/common/core/data_type.hpp"
#include "danejoe/common/enum/enum_convert.hpp"
#include "danejoe/network/codec/serialize_field.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @enum SerializeMapFlag
     * @brief 映射标志
     */
    enum class SerializeMapFlag :uint8_t
    {
        /// @brief 无标志
        None = 0,
        /// @brief 键长度是否可变
        IsKeyLengthVariable = 1 << 0,
        /// @brief 值长度是否可变
        IsValueLengthVariable = 1 << 1,
        /// @todo 其他的后续使用到再补充
    };
    /**
     * @brief 启用位掩码操作符
     */
    template<>
    struct enable_bitmask_operator<SerializeMapFlag> :std::true_type {};
    /**
     * @brief 获取枚举字符串
     * @param flag 枚举标志
     * @note 用于日志输出
     */
    std::string to_string(DaneJoe::SerializeMapFlag flag);
    /**
     * @brief 从字符串转换为枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 SerializeMapFlag::None
     */
    template<>
    SerializeMapFlag enum_cast<SerializeMapFlag>(const std::string& enum_string);
    /**
     * @struct SerializeMapValue
     * @brief 映射值结构体
     * @brief 由线格式转换，禁止直接拷贝
     * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
     * @example 消息结构:[键类型|值类型|元素对数量|键长度|值长度|键长度|值长度|键|值|键|值|]
     * @example 消息结构:[键类型|值类型|元素对数量|键长度|值长度|键|值|键|值|键|值|键|值|]
     */
    struct SerializeMapValue
    {
        /// @brief 键类型
        DataType key_type;
        /// @brief 值类型
        DataType value_type;
        /// @brief 元素对数量
        uint32_t element_count;
        /// @brief  区分定长和不定长的键值对，使用不同的方式处理
        SerializeMapFlag flag;
        /// @brief  当所有键的长度一致时，只有一个元素
        std::vector<uint32_t> key_length;
        /// @brief  当所有值的长度一致时，只有一个元素
        std::vector<uint32_t> value_length;
        /// @brief 键存储
        std::vector<uint8_t> key_value;
        /// @brief 值存储
        std::vector<uint8_t> value_value;
        /**
         * @brief 原始数据的最小长度
         */
        static uint32_t min_serialized_byte_array_size();
        /**
         * @brief 获取结构体所有成员序列化需要的大小
         */
        uint32_t serialized_size()const;
        /**
         * @brief 获取结构体的序列化数据
         */
        std::vector<uint8_t> to_serialized_byte_array()const;
        /**
         * @brief 从序列化数据获取结构体
         * @param data 序列化数据
         * @return 序列化数据对应的结构体
         */
        static std::optional<SerializeMapValue> from_serialized_byte_array(const std::vector<uint8_t>& data);
        /**
         * @brief 将结构体字符串化，便于调试输出
         * @return 结构体字符串
         */
        std::string to_string()const;
    };
    /**
     * @brief 获取map数据
     * @tparam Key 键类型
     * @tparam Value 值类型
     * @param field 字段
     * @return map数据
     */
    template <class Key, class Value>
    std::unordered_map<Key, Value> to_map(const DaneJoe::SerializeField& field)
    {
        /// @todo 后续完善
    }
}

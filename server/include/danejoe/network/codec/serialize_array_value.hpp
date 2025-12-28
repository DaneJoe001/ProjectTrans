/**
 * @file serialize_array_value.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数组值序列化
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/common/core/data_type.hpp"
#include "danejoe/common/binary/byte_array.hpp"
#include "danejoe/common/binary/byte_order.hpp"
#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/enum/enum_convert.hpp"
#include "danejoe/network/codec/serialize_field.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @enum SerializeArrayFlag
     * @brief 数组标志
     */
    enum class SerializeArrayFlag :uint8_t
    {
        /// @brief 无标志
        None = 0,
        /// @brief 元素类型长度是否可变
        IsElementLengthVariable = 1 << 0,
        /// @todo 其他的后续使用到再补充
    };
    /**
     * @brief 启用位掩码操作符
     */
    template<>
    struct enable_bitmask_operator<SerializeArrayFlag> :std::true_type {};
    /**
     * @brief 获取枚举字符串
     * @param flag 枚举标志
     * @note 用于日志输出
     */
    std::string to_string(DaneJoe::SerializeArrayFlag flag);
    /**
     * @brief 从字符串转换为枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 SerializeArrayFlag::None
     */
    template<>
    SerializeArrayFlag enum_cast<SerializeArrayFlag>(const std::string& enum_string);
    /**
     * @brief 从Filed中提取的value进行解析得到该结构
     * @brief 由线格式转换，禁止直接拷贝
     * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
     * @note
     * @note 数组元素必须要求提前设置好元素长度列表和元素数据
     * @example 消息结构:[元素类型|元素数量|数组标志|元素长度|元素值]
     * @example 消息结构:[元素类型|元素数量|数组标志|元素长度|元素长度|元素值|元素值]
     */
    struct SerializeArrayValue
    {
        /// @brief 元素类型
        DataType element_type;
        /// @brief 元素数量
        uint32_t element_count;
        /// @brief 区分数组元素是否定长等用途
        SerializeArrayFlag flag;
        /// @brief 当数组元素长度相等时，只需要一个长度
        std::vector<uint32_t> element_value_length;
        /// @brief 数组元素值
        std::vector<uint8_t> element_value;
        /**
         * @brief 原始数据的最小长度
         */
        static uint32_t min_serialized_byte_array_size();
        /**
         * @brief 获取结构体所有成员序列化需要的大小
         * @note 包含可变长度部分
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
        static std::optional<SerializeArrayValue> from_serialized_byte_array(const std::vector<uint8_t>& data);
        /**
         * @brief 将结构体字符串化，便于调试输出
         */
        std::string to_string()const;
    };
    /**
     * @brief 获取数组数据
     * @brief 变长数组约束返回std::vector<uint8_t>为元素的嵌套数组
     * @param field 字段
     * @return 数组数据
     */
    template<typename T>
    std::vector<T> to_array(const DaneJoe::SerializeField& field)
    {
        auto array_op = DaneJoe::SerializeArrayValue::from_serialized_byte_array(field.value);
        if (!array_op.has_value())
        {
            ADD_DIAG_ERROR("network", "to_array from_serialized_byte_array failed");
            return std::vector<T>();
        }
        DaneJoe::SerializeArrayValue array_value = array_op.value();
        if (array_value.element_type != get_data_type<T>())
        {
            ADD_DIAG_ERROR("network", "to_array element_type not match");
            return std::vector<T>();
        }
        std::vector<T> result;
        if (has_flag(array_value.flag, DaneJoe::SerializeArrayFlag::IsElementLengthVariable))
        {
            if (array_value.element_count != array_value.element_value_length.size())
            {
                ADD_DIAG_ERROR("network", "to_array element_count not match element_value_length size");
                return std::vector<T>();
            }
            if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
            {
                uint32_t current_index = 0;
                for (uint32_t i = 0; i < array_value.element_count; ++i)
                {
                    std::vector<uint8_t> element_value = to_byte_array<uint8_t>(array_value.element_value.data() + current_index, array_value.element_value_length[i]);
                    result.push_back(element_value);
                    current_index += array_value.element_value_length[i];
                }
            }
            else
            {
                return std::vector<T>();
            }
        }
        else
        {
            // 此处是指定元素数组不需要乘以元素大小
            result.resize(array_value.element_count);
            uint32_t current_index = 0;
            for (uint32_t i = 0; i < array_value.element_count; ++i)
            {
                to_local_byte_order(reinterpret_cast<uint8_t*>(&(result[i])), reinterpret_cast<const T*>(array_value.element_value.data() + current_index));
                current_index += sizeof(T);
            }
        }
        return result;
    }
}

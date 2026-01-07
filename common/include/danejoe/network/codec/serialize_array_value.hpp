/**
 * @file serialize_array_value.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数组值序列化
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义数组值的序列化结构 SerializeArrayValue 以及相关辅助函数。
 *          该模块用于在 SerializeField::value 中承载“同类型数组”的编码结果，并提供反序列化与提取工具。
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
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum SerializeArrayFlag
     * @brief 数组标志
     * @details 描述数组元素长度/布局等编码特征。
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
     * @details 为 SerializeArrayFlag 启用按位运算符重载（见 enum_flag.hpp）。
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
     * @struct SerializeArrayValue
     * @brief 数组值序列化结构
     * @details 用于表达同类型数组在网络字段中的编码形式。
     *          SerializeField::value 通常可被解析为该结构。
     *
     *          编码结构（逻辑顺序）：
     *          - element_type | element_count | flag | element_length... | element_value...
     *
     *          当 flag 不包含 SerializeArrayFlag::IsElementLengthVariable 时：
     *          - element_value_length 只包含 1 个长度（所有元素定长）
     *          - element_value 为按元素顺序紧密排列的字节序列
     *
     *          当 flag 包含 SerializeArrayFlag::IsElementLengthVariable 时：
     *          - element_value_length 的长度应等于 element_count
     *          - element_value 为各元素字节串拼接后的结果
     *
     * @note 该结构中的 element_value 为原始字节数组；如何转换为具体类型由调用方决定。
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
     * @tparam T 数组元素类型
     * @param field 字段
     * @return 数组数据
     * @details 从 field.value 反序列化得到数组。
     *          - 当数组为定长元素（未设置 IsElementLengthVariable）时，按元素大小解析为 std::vector<T>
     *          - 当数组为变长元素（设置 IsElementLengthVariable）时，仅支持 T 为 std::vector<uint8_t>
     * @note 失败时返回空数组，并通过诊断系统记录错误。
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

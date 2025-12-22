/**
 * @file serialize_field.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化字段
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/common/core/data_type.hpp"
#include "danejoe/common/type_traits/enum_type_traits.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @enum 字段标志
     * @brief 判断对应位是否唯一
     */
    enum class SerializeFieldFlag :uint8_t
    {
        /// @brief 无标志
        None = 0,
        /// @brief 是否含有长度信息
        HasValueLength = 1 << 0,
        /// @todo 其他的后续使用到再补充
    };
    /**
     * @brief 启用位掩码操作符
     */
    template<>
    struct enable_bitmask_operator<SerializeFieldFlag> :std::true_type {};
    /**
     * @brief 获取枚举字符串
     * @param flag 枚举标志
     * @note 用于日志输出
     */
    std::string to_string(DaneJoe::SerializeFieldFlag flag);
    /**
     * @brief 从字符串转换为枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 SerializeFieldFlag::None
     */
    template<>
    SerializeFieldFlag enum_cast<SerializeFieldFlag>(const std::string& enum_string);
    /**
     * @struct SerializeField
     * @brief 字段结构体
     * @brief 由线格式转换，禁止直接拷贝
     * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
     */
    struct SerializeField
    {
        /// @brief 字段名长度
        uint16_t name_length;
        /// @brief 用于标识字段名
        std::vector<uint8_t> name;
        /// @brief 字段类型
        DataType type;
        /// @brief 保留一位用于区分是否添加值长度
        SerializeFieldFlag flag;
        /// @brief 字段值长度（当字段宽度一定时，无效）
        uint32_t value_length;
        /// @brief 字段值
        std::vector<uint8_t> value;
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
        static std::optional<SerializeField> from_serialized_byte_array(const std::vector<uint8_t>& data);
        /**
         * @brief 将结构体字符串化，便于调试输出
         * @return 结构体字符串
         */
        std::string to_string()const;
        /**
         * @brief 返回原始类型数据
         * @note 返回不同类型的数据
         * @return 原始类型数据
         */
        std::vector<uint8_t> get_value()const;
    };
    /**
     * @brief 获取字符串数据
     * @param field 字段
     * @return 字符串
     */
    std::string to_string(const DaneJoe::SerializeField& field);
    /**
     * @brief 返回原始类型数据
     * @note 返回不同类型的数据
     * @tparam T 需要转换的数据类型
     * @param field 字段
     * @return 转换后的数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    std::optional<T> to_value(const DaneJoe::SerializeField& field)
    {
        DataType dest_type = get_data_type<T>();
        /// @todo 字节序转换
        if (field.type != dest_type)
        {
            ADD_DIAG_ERROR("network", "to_value field type not match");
            return std::nullopt;
        }
        return try_cast<T>(field.value);
    }
}

/**
 * @file serialize_header.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化头
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

#include "danejoe/common/type_traits/enum_type_traits.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 消息标志位
     */
    enum class SerializeFlag :uint8_t
    {
        /// @brief 无标志
        None = 0,
        /// @brief 是否含有校验码
        HasCheckSum = 1 << 0,
    };
    /**
     * @brief 启用位掩码操作符
     */
    template<>
    struct enable_bitmask_operator<SerializeFlag> :std::true_type {};
    /**
     * @brief 获取枚举字符串
     * @param flag 枚举标志
     * @note 用于日志输出
     */
    std::string to_string(DaneJoe::SerializeFlag flag);
    /**
     * @brief 从字符串转换为枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 SerializeFlag::None
     */
    template<>
    SerializeFlag enum_cast<SerializeFlag>(const std::string& enum_string);
    /**
     * @struct SerializeHeader
     * @brief 消息头结构体
     * @note 对于字典类型，字段数量无效
     */
    struct SerializeHeader
    {
        /// @brief 魔数（用于协议标识）
        uint32_t magic_number = 0x66666666;
        /// @brief 版本号
        uint8_t version = 1;
        /// @brief 消息体长度，不包括自身长度
        uint32_t message_length;
        /// @brief 消息标志
        SerializeFlag flag;
        /// @brief 校验和
        uint32_t checksum;
        /// @brief 当前层字段数量,不计入子对象
        uint16_t field_count;
        /**
         * @brief 原始数据的最小长度（固定部分，不含变长）
         */
        static uint32_t min_serialized_byte_array_size();
        /**
         * @brief 获取结构体所有成员序列化需要的大小
         * @note 包含可变长度部分
         */
        uint32_t serialized_size()const;
        /**
         * @brief 获取结构体的序列化数据
         * @return 序列化数据
         */
        std::vector<uint8_t> to_serialized_byte_array()const;
        /**
         * @brief 从序列化数据获取结构体
         * @param data 序列化数据
         * @return 序列化数据对应的结构体
         */
        static std::optional<SerializeHeader> from_serialized_byte_array(const std::vector<uint8_t>& data);
        /**
         * @brief 将结构体字符串化，便于调试输出
         * @return 结构体字符串
         */
        std::string to_string()const;
    };
}

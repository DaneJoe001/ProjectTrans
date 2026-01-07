/**
 * @file serialize_header.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化头
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义序列化消息头 SerializeHeader 及其标志位 SerializeFlag。
 *          SerializeHeader 用于描述后续消息体的长度、字段数量与可选校验信息，并作为 SerializeCodec/FrameAssembler 的协议基础。
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
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @enum SerializeFlag
     * @brief 消息标志位
     * @details 描述消息头/消息体的附加特性（例如是否包含校验和）。
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
     * @details 为 SerializeFlag 启用按位运算符重载（见 enum_type_traits.hpp）。
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
     * @details 用于描述消息体的边界与特性。
     *          典型字段含义：
     *          - magic_number：协议标识
     *          - version：协议版本
     *          - message_length：消息体长度（不包含消息头本身）
     *          - flag：消息标志（例如 HasCheckSum）
     *          - checksum：可选校验和（是否有效由 flag 决定）
     *          - field_count：当前层字段数量（不计入子对象/子字典）
     * @note 对于字典类型等特殊编码形式，field_count 的含义可能由具体实现决定。
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
         * @return 最小序列化字节数
         */
        static uint32_t min_serialized_byte_array_size();
        /**
         * @brief 获取结构体所有成员序列化需要的大小
         * @note 包含可变长度部分
         * @return 序列化字节数
         */
        uint32_t serialized_size()const;
        /**
         * @brief 获取结构体的序列化数据
         * @return 序列化数据
         * @details 将消息头编码为字节序列（写入字节流时通常采用网络字节序，具体由实现决定）。
         */
        std::vector<uint8_t> to_serialized_byte_array()const;
        /**
         * @brief 从序列化数据获取结构体
         * @param data 序列化数据
         * @return 序列化数据对应的结构体
         * @details 若 data 长度不足或数据非法则返回 std::nullopt。
         */
        static std::optional<SerializeHeader> from_serialized_byte_array(const std::vector<uint8_t>& data);
        /**
         * @brief 将结构体字符串化，便于调试输出
         * @return 结构体字符串
         */
        std::string to_string()const;
    };
}

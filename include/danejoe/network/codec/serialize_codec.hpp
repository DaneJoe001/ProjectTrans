/**
 * @file serialize_codec.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化编码器
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <type_traits>

#include "danejoe/common/binary/byte_array.hpp"
#include "danejoe/common/core/data_type.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/codec/serialize_header.hpp"
#include "danejoe/network/codec/serialize_field.hpp"
#include "danejoe/network/codec/i_serialize_dictionary.hpp"
#include "danejoe/network/codec/serialize_array_value.hpp"
#include "danejoe/network/codec/serialize_config.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class SerializeCodec
     * @brief 序列化类
     * @note 序列化数据默认都为大端存储（默认网络字节序）
     * @note 内部在序列化前，不会对字段长度会进行检查（外部建议做出保证）
     * @note 统一规定，存储到field结构体中的是local字节序,存储到字节流中的是network字节序
     * @todo 当前未使用版本以及校验和反序列化最终长度检查等实现，后续再引入
     */
    class SerializeCodec
    {
    public:
        /**
         * @brief 构造函数
         * @param config 序列化配置
         */
        SerializeCodec(const SerializeConfig& config = SerializeConfig());
        /**
         * @brief 设置序列化配置
         * @param config 序列化配置
         */
        void set_config(const SerializeConfig& config);
        /**
         * @brief 重置构建容器和构建映射
         */
        void reset_build();
        /**
         * @brief 重置解析容器和解析映射
         */
        void reset_parse();
        /**
         * @brief 更新消息头信息
         */
        void finalize_message_header();
        /**
         * @brief 获取消息头
         * @param data 序列化数据
         * @return 消息头
         */
        std::optional<SerializeHeader> get_message_header(const std::vector<uint8_t>& data)const noexcept;
        /**
         * @brief 获取解析的字段
         * @param key 字段名
         * @note 保证生命周期安全，不使用指针返回
         * @return 字段
         */
        std::optional<SerializeField> get_parsed_field(const std::string& key)const;
        /**
         * @brief 序列化好的字节流数据
         */
        std::vector<uint8_t> get_serialized_data_vector_build()noexcept;
        /**
         * @brief 反序列化字节流数据
         */
        std::vector<uint8_t> get_serialized_data_vector_parsed()const noexcept;
        /**
         * @brief 确保写入序列化时有足够的空间
         */
        void ensure_enough_capacity_rest_to_build(uint32_t size_need);
        /**
         * @brief 添加数据块到要解析容器中
         * @param data 数据块
         */
        void add_data_block_to_parse(const std::vector<uint8_t>& data);
        /**
         * @brief 判断帧是否完整
         * @note 当帧出现问题时，会对已接收部分进行重置
         */
        bool is_frame_complete();
        /**
         * @brief 反序列化字节流数据,并存储至内部映射
         */
        void deserialize(const std::vector<uint8_t>& data);
        /**
         * @brief 反序列化字节流数据(字符串传递),并存储至内部映射
         */
        void deserialize(const std::string& data);
        /**
         * @brief 解析内部反序列化数据
         */
        void deserialize();
        /**
         * @brief 序列化字节流数据，可递归调用
         * @param filed 字段
         * @return 返回当前对象进行递归调用
         * @note 该函数参数为封装好的字段
         * @todo 对具体的序列化实现模板函数和特化模板（非封装字段）
         */
        SerializeCodec& serialize(const SerializeField& field);
        /**
         * @brief 字典式序列化
         * @param data_name 字典名称（用于标识不同实现字典）
         * @param data 字典数据
         * @return 返回当前对象进行递归调用
         */
        SerializeCodec& serialize(const std::string& data_name, const ISerializeDictionary& data);
        /**
         * @brief 序列化模板
         * @tparam T 需要序列化的数据类型（非指针）
         * @param data 序列化数据
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         */
        template <class T>
        SerializeCodec& serialize(const T& data, const std::string& data_name)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, char*>)
            {
                return serialize(std::string(data), data_name);
            }
            SerializeField field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            field.type = get_data_type<T>();
            field.flag = SerializeFieldFlag::None;
            field.value = to_byte_array(data);
            field.value_length = field.value.size();
            return serialize(field);
        }
        /**
         * @brief 序列化模板
         * @tparam T 需要序列化的数据类型（指针）
         * @param data 序列化数据
         * @param size 数据数量
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         * @note T本身是指针类型当作变长数组处理
         * @note 考虑T本身可能是容器，当前没有做出处理
         */
        template <class T>
        SerializeCodec& serialize(const T* data, uint32_t size, const std::string& data_name)
        {
            if (data == nullptr || size == 0)
            {
                ADD_DIAG_WARN("network", "Serialize data skipped: data is null or size is 0");
                return *this;
            }
            SerializeField field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            if (size > 1)
            {
                // char*的多元素数据按字符串类型进行处理
                // 需要区分一下字节流
                if (std::is_same_v<T, char>)
                {
                    field.type = DaneJoe::DataType::String;
                }
                else
                {
                    field.type = DaneJoe::DataType::Array;
                }
                // 数组和字符串类型使用长度信息
                field.flag = SerializeFieldFlag::HasValueLength;
            }
            else
            {
                field.type = get_data_type<T>();
                // 普通类型不使用长度信息
                field.flag = SerializeFieldFlag::None;
            }
            if (field.type == DaneJoe::DataType::Array)
            {
                SerializeArrayValue array_value;
                array_value.element_type = get_data_type<T>();
                array_value.element_count = size;
                // 此处不判断是否变长
                /// @todo 对于ptr**和嵌套容器如std::string<std::string>去单独实现模板变长
                array_value.flag = SerializeArrayFlag::None;
                array_value.element_value_length.push_back(sizeof(T));
                // 此处是字节流数组
                array_value.element_value.resize(sizeof(T) * size);
                uint32_t current_index = 0;

                for (uint32_t i = 0; i < size; i++)
                {
                    to_network_byte_order(array_value.element_value.data() + current_index, data[i]);
                    current_index += sizeof(T);
                }
                field.value = array_value.to_serialized_byte_array();
            }
            else
            {
                field.value = to_byte_array<T>(data, size);
            }
            field.value_length = field.value.size();
            return serialize(field);
        }
        /**
         * @brief 序列化模板
         * @tparam T 需要序列化的数据类型
         * @param data 序列化数据
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         */
        template <class T>
        SerializeCodec& serialize(const std::vector<T>& data, const std::string& data_name)
        {
            return serialize(data.data(), data.size(), data_name);
        }
        /**
         * @brief 序列化模板
         * @tparam Key 键类型
         * @tparam Value 值类型
         * @param data 序列化数据
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         */
        template <class Key, class Value>
        SerializeCodec& serialize(const std::map<Key, Value>& data, const std::string& data_name)
        {
            /// @todo 后续再完善，当前不做处理
            return *this;
        }
        /**
         * @brief 序列化模板
         * @param data 序列化数据
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         */
        SerializeCodec& serialize(const std::string& data, const std::string& data_name)
        {
            SerializeField field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            field.type = DaneJoe::DataType::String;
            field.flag = SerializeFieldFlag::HasValueLength;
            field.value = std::vector<uint8_t>(data.begin(), data.end());
            field.value_length = field.value.size();
            return serialize(field);
        }
        /**
         * @brief 获取消息头大小
         * @return 消息头大小
         */
        static uint16_t get_message_header_size();
        /**
         * @brief 获取解析的字段映射表
         * @return 返回字段信息映射表
         */
        std::unordered_multimap<std::string, SerializeField> get_parsed_data_map()const noexcept;
    private:
        /// @brief 头部大小固定16字节
        static const uint32_t HEADER_SIZE;
    private:
        /// @brief 接收到的序列化字节流解析的当前下标位置
        /// @note 用于判断消息头判断，以及长度判断
        uint32_t m_parsed_buffer_index = 0;
        /// @brief 完成解析的字节流
        SerializeHeader m_parsed_message_header;
        /// @brief 是否已经完成解析消息头
        bool m_is_parsed_header_finished = false;
        /// @brief 构建字节流当前的下标位置，已信息头结束位置开始
        uint32_t m_current_index = HEADER_SIZE;
        /// @brief 逐步构建的序列化字节流
        std::vector<uint8_t> m_serialized_byte_array_build;
        /// @brief 接收到的序列化字节流
        std::vector<uint8_t> m_serialized_byte_array_parsed;
        /// @brief 构建的序列化字节流映射
        std::unordered_multimap<std::string, SerializeField> m_serialized_data_map_build;
        /// @brief 接收到的序列化字节流映射
        std::unordered_multimap<std::string, SerializeField> m_serialized_data_map_parsed;
        /// @brief 序列化配置
        SerializeConfig m_serialized_config;
    };
}

#pragma once

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <algorithm>

#include "common/util/enum_util.hpp"
#include "common/util/byte_array_util.hpp"
#include "common/util/data_type_util.hpp"

namespace DaneJoe
{

    /**
     * @brief 字典接口
     */
    class IDictionary
    {
    public:
        /**
         * @brief 虚析构
         */
        virtual ~IDictionary() = default;
        /**
         * @brief 获取序列化数据
         * @return 序列化数据
         */
        virtual std::vector<uint8_t> serialize() = 0;
        /**
         * @brief 解析序列化数据
         * @param 序列化数据
         */
        virtual void deserialize(const std::vector<uint8_t>& data_to_parse) = 0;
    };
    /**
     * @class DaneJoeSerializer
     * @brief 序列化类
     * @note 序列化数据默认都为大端存储（默认网络字节序）
     * @note 内部在序列化前，不会对字段长度会进行检查（外部建议做出保证）
     * @note 统一规定，存储到field结构体中的是local字节序,存储到字节流中的是network字节序
     * @todo 当前未使用版本以及校验和反序列化最终长度检查等实现，后续再引入
     */
    class DaneJoeSerializer
    {
    public:
        /**
         * @brief 消息标志位
         */
        enum class MessageFlag :uint8_t
        {
            /// @brief 无标志
            None = 0,
            /// @brief 是否含有校验码
            HasCheckSum = 1 << 0,
        };
        /**
         * @enum 字段标志
         * @brief 判断对应位是否唯一
         */
        enum class FieldFlag :uint8_t
        {
            /// @brief 无标志
            None = 0,
            /// @brief 是否含有长度信息
            HasValueLength = 1 << 0,
            /// @todo 其他的后续使用到再补充
        };
        enum class ArrayFlag :uint8_t
        {
            /// @brief 无标志
            None = 0,
            /// @brief 元素类型长度是否可变
            IsElementLengthVariable = 1 << 0,
            /// @todo 其他的后续使用到再补充
        };
        enum class MapFlag :uint8_t
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
         * @struct 消息头结构体
         * @note 对于字典类型，字段数量无效
         */
        struct MessageHeader
        {
            /// @brief 魔数（用于协议标识）
            uint32_t magic_number = 0x66666666;
            /// @brief 版本号
            uint8_t version = 1;
            /// @brief 消息体长度，不包括自身长度
            uint32_t message_length;
            /// @brief 消息标志
            MessageFlag flag;
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
             */
            std::vector<uint8_t> to_serialized_byte_array()const;
            /**
             * @brief 从序列化数据获取结构体
             */
            static std::optional<MessageHeader> from_serialized_byte_array(const std::vector<uint8_t>& data);
            /**
             * @brief 将结构体字符串化，便于调试输出
             */
            std::string to_string()const;
        };
        /**
         * @struct 字段结构体
         * @brief 由线格式转换，禁止直接拷贝
         * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
         */
        struct Field
        {
            /// @brief 字段名长度
            uint16_t name_length;
            /// @brief 用于标识字段名
            std::vector<uint8_t> name;
            /// @brief 字段类型
            DataType type;
            /// @brief 保留一位用于区分是否添加值长度
            FieldFlag flag;
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
             */
            static std::optional<Field> from_serialized_byte_array(const std::vector<uint8_t>& data);
            /**
             * @brief 将结构体字符串化，便于调试输出
             */
            std::string to_string()const;
            /**
             * @brief 返回原始类型数据
             * @note 返回不同类型的数据
             */
            std::vector<uint8_t> get_value()const;
        };
        /**
         * @brief 从Filed中提取的value进行解析得到该结构
         * @brief 由线格式转换，禁止直接拷贝
         * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
         * @note
         * @note 数组元素必须要求提前设置好元素长度列表和元素数据
         * @example 消息结构:[元素类型|元素数量|数组标志|元素长度|元素值]
         * @example 消息结构:[元素类型|元素数量|数组标志|元素长度|元素长度|元素值|元素值]
         */
        struct ArrayValue
        {
            /// @brief 元素类型
            DataType element_type;
            /// @brief 元素数量
            uint32_t element_count;
            /// @brief 区分数组元素是否定长等用途
            ArrayFlag flag;
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
             */
            static std::optional<ArrayValue> from_serialized_byte_array(const std::vector<uint8_t>& data);
            /**
             * @brief 将结构体字符串化，便于调试输出
             */
            std::string to_string()const;
        };
        /**
         * @brief 从Filed中提取的value进行解析得到该结构
         * @brief 由线格式转换，禁止直接拷贝
         * @note 该结构中的数据均是字节数组，需要自行转换成对应的数据类型
         * @example 消息结构:[键类型|值类型|元素对数量|键长度|值长度|键长度|值长度|键|值|键|值|]
         * @example 消息结构:[键类型|值类型|元素对数量|键长度|值长度|键|值|键|值|键|值|键|值|]
         */
        struct MapValue
        {
            /// @brief 键类型
            DataType key_type;
            /// @brief 值类型
            DataType value_type;
            /// @brief 元素对数量
            uint32_t element_count;
            /// @brief  区分定长和不定长的键值对，使用不同的方式处理
            MapFlag flag;
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
             */
            static std::optional<MapValue> from_serialized_byte_array(const std::vector<uint8_t>& data);
            /**
             * @brief 将结构体字符串化，便于调试输出
             */
            std::string to_string()const;
        };
        /**
         * @struct SerializedConfig
         * @brief 序列化配置
         */
        struct SerializedConfig
        {
            uint32_t max_field_value_length = 1 * 1024 * 1024;
            uint32_t max_field_name_length = 128;
            uint32_t pre_allocated_size = 4 * 1024;
            SerializedConfig();
        };
    public:
        DaneJoeSerializer(const SerializedConfig& config = SerializedConfig());
        /**
         * @brief 设置序列化配置
         */
        void set_config(const SerializedConfig& config);
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
         */
        std::optional<MessageHeader> get_message_header(const std::vector<uint8_t>& data)const noexcept;
        /**
         * @brief 获取解析的字段
         * @param key 字段名
         * @note 保证生命周期安全，不使用指针返回
         * @return 字段
         */
        std::optional<Field> get_parsed_field(const std::string& key)const;
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
         * @brief 反序列化字节流数据,并存储至内部映射
         */
        void deserialize(const std::vector<uint8_t>& data);
        /**
         * @brief 反序列化字节流数据(字符串传递),并存储至内部映射
         */
        void deserialize(const std::string& data);
        /**
         * @brief 序列化字节流数据，可递归调用
         * @param filed 字段
         * @note 该函数参数为封装好的字段
         * @todo 对具体的序列化实现模板函数和特化模板（非封装字段）
         */
        DaneJoeSerializer& serialize(const Field& field);
        /**
         * @brief 字典式序列化
         * @param data_name 字典名称（用于标识不同实现字典）
         * @param data 字典数据
         */
        DaneJoeSerializer& serialize(const std::string& data_name, const IDictionary& data);
        /**
         * @brief 序列化模板
         * @tparam T 需要序列化的数据类型（非指针）
         * @param data 序列化数据
         * @param data_name 字段名称
         * @return 返回当前对象进行递归调用
         */
        template <class T>
        DaneJoeSerializer& serialize(const T& data, const std::string& data_name)
        {
            Field field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            field.type = get_data_type<T>();
            field.flag = FieldFlag::None;
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
        DaneJoeSerializer& serialize(const T* data, uint32_t size, const std::string& data_name)
        {
            if (data == nullptr || size == 0)
            {
                return *this;
            }
            Field field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            if (size > 1)
            {
                // char*的多元素数据按字符串类型进行处理
                if (std::is_same_v<T, char>)
                {
                    field.type = DaneJoe::DataType::String;
                }
                else
                {
                    field.type = DaneJoe::DataType::Array;
                }
                // 数组和字符串类型使用长度信息
                field.flag = FieldFlag::HasValueLength;
            }
            else
            {
                field.type = get_data_type<T>();
                // 普通类型不使用长度信息
                field.flag = FieldFlag::None;
            }
            if (field.type == DaneJoe::DataType::Array)
            {
                ArrayValue array_value;
                array_value.element_type = get_data_type<T>();
                array_value.element_count = size;
                // 此处不判断是否变长
                /// @todo 对于ptr**和嵌套容器如std::string<std::string>去单独实现模板变长
                array_value.flag = ArrayFlag::None;
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
        template <class T>
        DaneJoeSerializer& serialize(const std::vector<T>& data, const std::string& data_name)
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
        DaneJoeSerializer& serialize(const std::map<Key, Value>& data, const std::string& data_name)
        {
            /// @todo 后续再完善，当前不做处理
            return *this;
        }
        DaneJoeSerializer& serialize(const std::string& data, const std::string& data_name)
        {
            Field field;
            field.name = std::vector<uint8_t>(data_name.begin(), data_name.end());
            field.name_length = field.name.size();
            field.type = DaneJoe::DataType::String;
            field.flag = FieldFlag::HasValueLength;
            field.value = std::vector<uint8_t>(data.begin(), data.end());
            field.value_length = field.value.size();
            return serialize(field);
        }
    private:
        /// @brief 头部大小固定16字节
        const uint32_t HEADER_SIZE = 16;
    private:
        /// @brief 构建字节流当前的下标位置，已信息头结束位置开始
        uint32_t m_current_index = HEADER_SIZE;
        /// @brief 逐步构建的序列化字节流
        std::vector<uint8_t> m_serialized_byte_array_build;
        /// @brief 接收到的序列化字节流
        std::vector<uint8_t> m_serialized_byte_array_parsed;
        /// @brief 构建的序列化字节流映射
        std::unordered_map<std::string, Field> m_serialized_data_map_build;
        /// @brief 接收到的序列化字节流映射
        std::unordered_map<std::string, Field> m_serialized_data_map_parsed;
        /// @brief 序列化配置
        SerializedConfig m_serialized_config;

    };

    /**
     * @brief 获取数组数据
     * @brief 变长数组约束返回std::vector<uint8_t>为元素的嵌套数组
     */
    template<typename T>
    std::vector<T> to_array(const DaneJoeSerializer::Field& field)
    {
        auto array_op = DaneJoeSerializer::ArrayValue::from_serialized_byte_array(field.value);
        if (!array_op.has_value())
        {
            return std::vector<T>();
        }
        DaneJoeSerializer::ArrayValue array_value = array_op.value();
        if (array_value.element_type != get_data_type<T>())
        {
            return std::vector<T>();
        }
        std::vector<T> result;
        if (has_flag(array_value.flag, DaneJoeSerializer::ArrayFlag::IsElementLengthVariable))
        {
            if (array_value.element_count != array_value.element_value_length.size())
            {
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
    /**
     * @brief 获取map数据
     */
    template <class Key, class Value>
    std::unordered_map<Key, Value> to_map(const DaneJoeSerializer::Field& field)
    {
        /// @todo 后续完善
    }
    /**
     * @brief 获取字符串数据
     * @param field 字段
     */
    std::string to_string(const DaneJoeSerializer::Field& field);
    /**
     * @brief 获取字典原始数据
     * @param field 字段
     */
    std::vector<uint8_t> to_dictionary(const DaneJoe::DaneJoeSerializer::Field& field);
    /**
     * @brief 返回原始类型数据
     * @note 返回不同类型的数据
     */
    template <class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>, int>>
    std::optional<T> to_value(const DaneJoeSerializer::Field& field)
    {
        DataType dest_type = get_data_type<T>();
        /// @todo 字节序转换
        if (field.type != dest_type)
        {
            return std::nullopt;
        }
        return to_value<T>(field.value);
    }

}

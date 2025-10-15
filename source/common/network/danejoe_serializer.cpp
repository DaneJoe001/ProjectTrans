#include <iostream>
#include <sstream>

#include "common/network/danejoe_serializer.hpp"
#include "common/util/print_util.hpp"

DaneJoe::DaneJoeSerializer::SerializedConfig::SerializedConfig()
    : max_field_value_length(1 * 1024 * 1024),
    max_field_name_length(128),
    pre_allocated_size(4 * 1024)
{
}

uint16_t DaneJoe::DaneJoeSerializer::get_message_header_size()
{
    return HEADER_SIZE;
}

uint32_t DaneJoe::DaneJoeSerializer::MessageHeader::min_serialized_byte_array_size()
{
    uint32_t size
        = sizeof(magic_number)// 获取魔数所占字节
        + sizeof(version)// 获取版本号长度所占字节
        + sizeof(message_length)// 获取消息长度信息所占字节
        + sizeof(flag)// 获取标志所占字节
        + sizeof(checksum)// 获取校验码所占字节
        + sizeof(field_count);// 获取块数量所占字节
    return size;
}

uint32_t DaneJoe::DaneJoeSerializer::MessageHeader::serialized_size()const
{
    // 序列化长度与最小内容所占长度一致（无变长部分）
    return min_serialized_byte_array_size();
}

std::optional<DaneJoe::DaneJoeSerializer::MessageHeader> DaneJoe::DaneJoeSerializer::MessageHeader::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    MessageHeader header;
    // 检查源长度是否合法
    if (data.size() < min_serialized_byte_array_size())
    {
        return std::nullopt;
    }
    // 源起始坐标
    uint32_t current_index = 0;
    // 获取魔数，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.magic_number)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(magic_number);
    // 获取版本，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.version)), reinterpret_cast<const uint8_t*>(data.data() + current_index));
    current_index += sizeof(version);
    // 获取长度部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.message_length)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(message_length);
    // 获取标志部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.flag)), reinterpret_cast<const MessageFlag*>(data.data() + current_index));
    current_index += sizeof(MessageFlag);
    // 获取校验码部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.checksum)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(checksum);
    // 获取块数量部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.field_count)), reinterpret_cast<const uint16_t*>(data.data() + current_index));
    current_index += sizeof(field_count);
    return header;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::MessageHeader::to_serialized_byte_array()const
{
    std::vector<uint8_t> data(serialized_size());
    uint32_t current_need_size = 0;
    uint32_t current_index = 0;
    // 写入魔数
    current_need_size += sizeof(magic_number);
    to_network_byte_order(data.data() + current_index, magic_number);
    current_index += sizeof(magic_number);
    // 写入版本
    current_need_size += sizeof(version);
    to_network_byte_order(data.data() + current_index, version);
    current_index += sizeof(version);
    // 写入长度
    current_need_size += sizeof(message_length);
    to_network_byte_order(data.data() + current_index, message_length);
    current_index += sizeof(message_length);
    // 写入标志
    current_need_size += sizeof(flag);
    to_network_byte_order(data.data() + current_index, flag);
    current_index += sizeof(flag);
    // 写入校验码
    current_need_size += sizeof(checksum);
    to_network_byte_order(data.data() + current_index, checksum);
    current_index += sizeof(checksum);
    // 写入块数量
    current_need_size += sizeof(field_count);
    to_network_byte_order(data.data() + current_index, field_count);
    current_index += sizeof(field_count);
    return data;
}

std::string DaneJoe::DaneJoeSerializer::MessageHeader::to_string()const
{
    std::ostringstream oss;
    oss << "MessageHeader: {"
        << " magic_number: " << magic_number
        << ", version: " << static_cast<int>(version) // 使用 static_cast<int> 以确保正确输出 uint8_t
        << ", message_length: " << message_length
        << ", flag: " << DaneJoe::to_string(flag)
        << ", checksum: " << checksum
        << ", field_count: " << field_count
        << " }"; // 结束大括号

    return oss.str(); // 返回拼接后的字符串
}

uint32_t DaneJoe::DaneJoeSerializer::Field::min_serialized_byte_array_size()
{
    uint32_t size
        = sizeof(name_length)// 名称长度所占字节
        + sizeof(type)// 数据类型所占字节
        + sizeof(flag);// 标志所占字节
    return size;
}

uint32_t DaneJoe::DaneJoeSerializer::Field::serialized_size()const
{
    uint32_t size
        = sizeof(name_length)// 名称长度所占字节
        + name.size()// 名称所占字节
        + sizeof(DataType)// 数据类型所占字节
        + sizeof(FieldFlag)// 标志所占字节
        + value.size();// 值所占字节
    if (has_flag(flag, FieldFlag::HasValueLength))
    {
        size += sizeof(value_length);// 值长度所占字节
    }
    return size;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::Field::get_value()const
{
    return value;
}

std::optional<DaneJoe::DaneJoeSerializer::Field> DaneJoe::DaneJoeSerializer::Field::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    uint32_t data_size = data.size();
    if (data_size < min_serialized_byte_array_size())
    {
        return std::nullopt;
    }
    Field field;
    uint32_t current_index = 0;
    uint32_t current_need_to_read_size = 0;
    // 获取名称长度字段
    current_need_to_read_size += sizeof(name_length);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.name_length)), reinterpret_cast<const uint16_t*>(data.data() + current_index));
    current_index += sizeof(name_length);
    // 获取名称字段
    current_need_to_read_size += field.name_length;
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        return std::nullopt;
    }
    field.name = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + field.name_length);
    current_index += field.name_length;
    // 获取数据类型字段
    current_need_to_read_size += sizeof(type);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.type)), reinterpret_cast<const DataType*>(data.data() + current_index));
    current_index += sizeof(type);
    // 获取标志字段
    current_need_to_read_size += sizeof(flag);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.flag)), reinterpret_cast<const FieldFlag*>(data.data() + current_index));
    current_index += sizeof(flag);
    // 获取值长度字段
    if (has_flag<FieldFlag>(field.flag, FieldFlag::HasValueLength))
    {
        current_need_to_read_size += sizeof(value_length);
        if (!ensure_enough_to_read(data, current_need_to_read_size))
        {
            return std::nullopt;
        }
        to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.value_length)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
        current_index += sizeof(value_length);
    }
    else
    {
        field.value_length = get_data_type_length(field.type);
    }
    // 获取值字段
    current_need_to_read_size += field.value_length;
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        return std::nullopt;
    }
    // 当类型为字符串/字节数组/数组/图/字典时直接写入数据
    /// @todo 当前没有考虑多字节字符类型（异端解析存在问题）
    if (field.type == DataType::String || field.type == DataType::ByteArray || field.type == DataType::Dictionary || field.type == DataType::Array || field.type == DataType::Map)
    {
        field.value = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + field.value_length);
    }
    else
    {
        field.value = to_local_byte_order(data.data() + current_index, field.value_length);
    }
    current_index += field.value_length;
    return field;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::Field::to_serialized_byte_array()const
{
    std::vector<uint8_t>data(serialized_size());
    uint32_t current_index = 0;
    uint32_t current_need_size = 0;
    // 写入名称长度字段
    current_need_size += sizeof(name_length);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, name_length);
    current_index += sizeof(name_length);
    // 写入名称字段
    current_need_size += name_length;
    ensure_enough_capacity(data, current_need_size);
    std::memcpy(data.data() + current_index, name.data(), name.size());
    current_index += name.size();
    // 写入数据类型字段
    current_need_size += sizeof(type);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, type);
    current_index += sizeof(type);
    // 写入标志字段
    current_need_size += sizeof(flag);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, flag);
    current_index += sizeof(flag);
    // 写入值长度字段
    if (has_flag(flag, FieldFlag::HasValueLength))
    {
        current_need_size += sizeof(value_length);
        ensure_enough_capacity(data, current_need_size);
        to_network_byte_order(data.data() + current_index, value_length);
        current_index += sizeof(value_length);
    }
    // 写入值字段
    current_need_size += value_length;
    ensure_enough_capacity(data, current_need_size);
    if (type == DataType::String || type == DataType::ByteArray || type == DataType::Array || type == DataType::Map || type == DataType::Dictionary)
    {
        std::memcpy(data.data() + current_index, value.data(), value.size());
    }
    else
    {
        to_network_byte_order(data.data() + current_index, value.data(), value_length);
    }
    current_index += value.size();
    return data;
}

std::string DaneJoe::DaneJoeSerializer::Field::to_string()const
{
    std::ostringstream oss;
    oss << "Field: {"
        << " name_length: " << name_length
        << ", name: " << std::string(name.begin(), name.end())
        << ", type: " << DaneJoe::to_string(type)
        << ", flag: " << DaneJoe::to_string(flag)
        << ", value size: " << value.size()
        << " }"; // 结束大括号

    return oss.str(); // 返回拼接后的字符串
}
uint32_t DaneJoe::DaneJoeSerializer::ArrayValue::min_serialized_byte_array_size()
{
    uint32_t size
        = sizeof(element_type)// 数据类型所占字节
        + sizeof(element_count)// 元素数量所占字节
        + sizeof(flag)// 标志所占字节
        + sizeof(uint32_t);// 元素长度列表所占字节
    return size;
}

uint32_t DaneJoe::DaneJoeSerializer::ArrayValue::serialized_size()const
{
    uint32_t size
        = sizeof(element_type)// 数据类型所占字节
        + sizeof(element_count)// 元素数量所占字节
        + sizeof(flag)// 标志所占字节
        + sizeof(uint32_t) * element_value_length.size()// 元素长度列表所占字节
        + sizeof(uint8_t) * element_value.size();// 元素所占字节
    return size;
}

std::optional<DaneJoe::DaneJoeSerializer::ArrayValue> DaneJoe::DaneJoeSerializer::ArrayValue::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    if (data.size() < min_serialized_byte_array_size())
    {
        return std::nullopt;
    }
    ArrayValue array_value;
    uint32_t current_index = 0;
    uint32_t current_need_size = 0;
    // 获取数据类型字段
    current_need_size += sizeof(element_type);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.element_type)), reinterpret_cast<const DataType*>(data.data() + current_index));
    current_index += sizeof(element_type);
    // 获取元素数量字段
    current_need_size += sizeof(element_count);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.element_count)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(element_count);
    // 获取标志字段
    current_need_size += sizeof(flag);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.flag)), reinterpret_cast<const ArrayFlag*>(data.data() + current_index));
    current_index += sizeof(flag);
    uint32_t value_length = 0;
    // 检查数组元素长度是否可变
    if (has_flag<ArrayFlag>(array_value.flag, ArrayFlag::IsElementLengthVariable))
    {
        // 写入元素长度列表
        array_value.element_value_length.resize(array_value.element_count);
        // 写入每个元素长度
        for (uint32_t i = 0; i < array_value.element_count; ++i)
        {
            current_need_size += sizeof(uint32_t);
            if (!ensure_enough_to_read(data, current_need_size))
            {
                return std::nullopt;
            }
            to_local_byte_order(reinterpret_cast<uint8_t*>(array_value.element_value_length.data() + i), reinterpret_cast<const uint32_t*>(data.data() + current_index));
            current_index += sizeof(uint32_t);
            value_length += array_value.element_value_length[i];
        }
    }
    else
    {
        // 不可变数组仅需写入一个元素长度
        current_need_size += sizeof(uint32_t);
        if (!ensure_enough_to_read(data, current_need_size))
        {
            return std::nullopt;
        }
        // 长度不变时写入一个长度信息
        array_value.element_value_length.resize(1);
        to_local_byte_order(reinterpret_cast<uint8_t*>(array_value.element_value_length.data()), reinterpret_cast<const uint32_t*>(data.data() + current_index));
        current_index += sizeof(uint32_t);
        value_length = array_value.element_value_length[0] * array_value.element_count;
    }
    // 写入值字段
    current_need_size += value_length;
    if (!ensure_enough_to_read(data, current_need_size))
    {
        return std::nullopt;
    }
    array_value.element_value = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + value_length);
    current_index += value_length;
    return array_value;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::ArrayValue::to_serialized_byte_array()const
{
    std::vector<uint8_t>data(serialized_size());
    bool is_variable_array = false;
    // 外部约束，当元素长度列表数量为1时代表定长数组
    // 定长数组只保留一个长度数据
    if (element_value_length.size() > 1)
    {
        is_variable_array = true;
    }
    // 当数组变长但长度数量不足时返回
    if (is_variable_array && element_count != element_value_length.size())
    {
        return std::vector<uint8_t>();
    }
    uint32_t current_index = 0;
    uint32_t current_need_size = 0;
    // 转化为网络字节序并插入容器对应位置
    current_need_size += sizeof(element_type);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, element_type);
    current_index += sizeof(element_type);
    // 转化为网络字节序并插入容器对应位置
    current_need_size += sizeof(element_count);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, element_count);
    current_index += sizeof(element_count);
    // 转化为网络字节序并插入容器对应位置
    current_need_size += sizeof(flag);
    ensure_enough_capacity(data, current_need_size);
    to_network_byte_order(data.data() + current_index, flag);
    current_index += sizeof(flag);

    if (is_variable_array)
    {
        for (uint32_t i = 0; i < element_count; ++i)
        {
            current_need_size += sizeof(element_value_length[i]);
            ensure_enough_capacity(data, current_need_size);
            to_network_byte_order(data.data() + current_index, element_value_length[i]);
            current_index += sizeof(element_value_length[i]);
        }
    }
    else
    {
        current_need_size += sizeof(uint32_t);
        ensure_enough_capacity(data, current_need_size);
        // 定长数组的长度也是通过std::vector<uint32_t>的数组保存，故源采用指针传递
        to_network_byte_order(data.data() + current_index, element_value_length.data());
        current_index += sizeof(uint32_t);
    }
    current_need_size += element_value.size();
    ensure_enough_capacity(data, current_need_size);
    std::memcpy(data.data() + current_index, element_value.data(), element_value.size());
    current_index += element_value.size();
    return data;
}

std::string DaneJoe::DaneJoeSerializer::ArrayValue::to_string()const
{
    std::ostringstream oss;
    oss << "ArrayValue: {"
        << " element_type: " << DaneJoe::to_string(element_type)
        << ", element_count: " << element_count
        << ", flag: " << DaneJoe::to_string(flag)
        << ", element_value_length size: " << element_value_length.size()
        << ", element_value size: " << element_value.size();
    oss << " }"; // 结束大括号

    return oss.str(); // 返回拼接后的字符串
}

uint32_t DaneJoe::DaneJoeSerializer::MapValue::serialized_size()const
{
    /// @todo 当前不做实现，后续再完善
    uint32_t size = 0;
    return size;
}

std::optional<DaneJoe::DaneJoeSerializer::MapValue> DaneJoe::DaneJoeSerializer::MapValue::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    /// @todo 后续实现
    return std::nullopt;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::MapValue::to_serialized_byte_array()const
{
    /// @todo 当前不做实现，后续再完善
    return std::vector<uint8_t>();
}

std::string DaneJoe::DaneJoeSerializer::MapValue::to_string()const
{
    /// @todo 当前不做实现，后续再完善
    return std::string("");
}


DaneJoe::DaneJoeSerializer::DaneJoeSerializer(const SerializedConfig& config) :m_serialized_config(config), m_serialized_byte_array_build(std::vector<uint8_t>(config.pre_allocated_size))
{
}

void DaneJoe::DaneJoeSerializer::set_config(const SerializedConfig& config)
{
    m_serialized_config = config;
    // 更新预分配容量
    if (m_serialized_byte_array_build.size() < config.pre_allocated_size)
    {
        m_serialized_byte_array_build.resize(config.pre_allocated_size);
    }
}

/**
 * @brief 重置构建容器和构建映射
 */
void DaneJoe::DaneJoeSerializer::reset_build()
{
    m_serialized_byte_array_build.clear();
    m_serialized_data_map_build.clear();
    m_current_index = HEADER_SIZE;
}

void DaneJoe::DaneJoeSerializer::reset_parse()
{
    m_serialized_byte_array_parsed.clear();
    m_serialized_data_map_parsed.clear();
    m_parsed_buffer_index = 0;
    m_is_parsed_header_finished = false;
}

std::vector<uint8_t>  DaneJoe::DaneJoeSerializer::get_serialized_data_vector_build()noexcept
{
    finalize_message_header();
    return m_serialized_byte_array_build;
}

std::vector<uint8_t> DaneJoe::DaneJoeSerializer::get_serialized_data_vector_parsed()const noexcept
{
    return m_serialized_byte_array_parsed;
}

void DaneJoe::DaneJoeSerializer::deserialize(const std::vector<uint8_t>& data)
{
    auto header_optional = get_message_header(data);
    if (!header_optional.has_value())
    {
        return;
    }
    // 消息长度超出数据长度
    auto header = header_optional.value();
    if (header.message_length + HEADER_SIZE > data.size())
    {
        return;
    }
    m_serialized_byte_array_parsed = data;
    uint32_t current_index = HEADER_SIZE;
    for (uint32_t i = 0; i < header.field_count; ++i)
    {
        Field field;
        // 读取名称字段长度
        to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.name_length)), reinterpret_cast<const uint16_t*>(data.data() + current_index));
        current_index += sizeof(uint16_t);
        // 读取名称字段
        field.name = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + field.name_length);
        current_index += field.name_length;
        // 读取字段类型
        to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.type)), reinterpret_cast<const uint8_t*>(data.data() + current_index));
        current_index += sizeof(uint8_t);
        // 读取字段标志
        to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.flag)), reinterpret_cast<const uint8_t*>(data.data() + current_index));
        current_index += sizeof(uint8_t);
        if (has_flag(field.flag, FieldFlag::HasValueLength))
        {
            // 读取值长度
            to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.value_length)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
            current_index += sizeof(uint32_t);
        }
        else
        {
            field.value_length = get_data_type_length(field.type);
        }
        // 读取字段值,字符串和字节数组直接写入数据
        if (field.type == DataType::String || field.type == DataType::ByteArray || field.type == DataType::Dictionary || field.type == DataType::Array || field.type == DataType::Map)
        {
            field.value = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + field.value_length);
        }
        else
        {
            field.value = to_local_byte_order_array<uint8_t>(data.data() + current_index, field.value_length);
        }
        current_index += field.value_length;
        // 插入字段
        m_serialized_data_map_parsed.insert(std::make_pair(std::string(field.name.begin(), field.name.end()), field));
    }
}

void DaneJoe::DaneJoeSerializer::deserialize(const std::string& data)
{
    std::vector<uint8_t> data_vector(data.begin(), data.end());
    deserialize(data_vector);
}

void DaneJoe::DaneJoeSerializer::deserialize()
{

}

DaneJoe::DaneJoeSerializer& DaneJoe::DaneJoeSerializer::serialize(const Field& field)
{
    if (field.name_length > m_serialized_config.max_field_name_length)
    {
        /// @todo Log
        return *this;
    }
    if (field.value_length > m_serialized_config.max_field_value_length)
    {
        /// @todo Log
        return *this;
    }
    uint32_t field_size = field.serialized_size();
    ensure_enough_capacity_rest_to_build(field_size);
    auto data = field.to_serialized_byte_array();
    std::memcpy(m_serialized_byte_array_build.data() + m_current_index, data.data(), data.size());
    m_current_index += field_size;
    m_serialized_data_map_build.insert(std::make_pair(std::string(field.name.begin(), field.name.end()), field));
    return *this;
}

void DaneJoe::DaneJoeSerializer::finalize_message_header()
{
    m_serialized_byte_array_build.resize(m_current_index);
    MessageHeader header;
    header.message_length = m_current_index - HEADER_SIZE;
    header.flag = MessageFlag::None;
    header.checksum = 0;
    header.field_count = m_serialized_data_map_build.size();

    uint32_t current_index = 0;
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.magic_number);
    current_index += sizeof(uint32_t);
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.version);
    current_index += sizeof(uint8_t);
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.message_length);
    current_index += sizeof(uint32_t);
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.flag);
    current_index += sizeof(uint8_t);
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.checksum);
    current_index += sizeof(uint32_t);
    to_network_byte_order(m_serialized_byte_array_build.data() + current_index, header.field_count);
    current_index += sizeof(uint16_t);
}

std::optional<DaneJoe::DaneJoeSerializer::MessageHeader> DaneJoe::DaneJoeSerializer::get_message_header(const std::vector<uint8_t>& data)const noexcept
{
    if (data.size() < HEADER_SIZE)
    {
        return std::nullopt;
    }
    auto header_optional = DaneJoeSerializer::MessageHeader::from_serialized_byte_array(std::vector<uint8_t>(data.begin(), data.begin() + HEADER_SIZE));
    return header_optional;
}

void DaneJoe::DaneJoeSerializer::ensure_enough_capacity_rest_to_build(uint32_t size_need)
{
    auto size = m_serialized_byte_array_build.size();
    auto request_total_size = m_current_index + size_need;
    if (request_total_size > size)
    {
        m_serialized_byte_array_build.resize(request_total_size);
    }
}

void DaneJoe::DaneJoeSerializer::add_data_block_to_parse(const std::vector<uint8_t>& data)
{
    // 先判断是否有足够的空间容量
    uint32_t size_need = m_parsed_buffer_index + data.size();
    if ( size_need> m_serialized_byte_array_parsed.size())
    {
        m_serialized_byte_array_parsed.resize(size_need);
    }
    for (const auto& byte : data)
    {
        m_serialized_byte_array_parsed[m_parsed_buffer_index++] = byte;
    }
}

bool DaneJoe::DaneJoeSerializer::is_frame_complete()
{
    if (m_parsed_buffer_index < HEADER_SIZE)
    {
        return false;
    }
    if (!m_is_parsed_header_finished)
    {
        auto header_optional = get_message_header(m_serialized_byte_array_parsed);
        // 当消息头解析失败时说明帧不完整，需要重置
        if (!header_optional.has_value())
        {
            reset_parse();
            return false;
        }
        m_parsed_message_header = header_optional.value();
        m_is_parsed_header_finished = true;
    }
    /// @todo 可能存在多个帧粘连的情况
    
    if (m_parsed_buffer_index < m_parsed_message_header.message_length + HEADER_SIZE)
    {
        return false;
    }
    return true;

}

DaneJoe::DaneJoeSerializer& DaneJoe::DaneJoeSerializer::serialize(const std::string& data_name, const IDictionary& data)
{
    /// @todo 后续再完善
    return *this;
}

std::optional<DaneJoe::DaneJoeSerializer::Field> DaneJoe::DaneJoeSerializer::get_parsed_field(const std::string& key)const
{
    auto field_it = m_serialized_data_map_parsed.find(key);
    if (field_it == m_serialized_data_map_parsed.end())
    {
        return std::nullopt;
    }
    return field_it->second;
}

std::unordered_multimap<std::string, DaneJoe::DaneJoeSerializer::Field> DaneJoe::DaneJoeSerializer::get_parsed_data_map()const noexcept
{
    return m_serialized_data_map_parsed;
}

std::string DaneJoe::to_string(const DaneJoeSerializer::Field& field)
{
    if (field.type == DataType::String)
    {
        return std::string(field.value.begin(), field.value.end());
    }
    else if (field.type == DataType::UInt8)
    {
        return DaneJoe::to_string(reinterpret_cast<const uint8_t*>(field.value.data()));
    }
    else if (field.type == DataType::Int8)
    {
        return DaneJoe::to_string(reinterpret_cast<const int8_t*>(field.value.data()));
    }
    else if (field.type == DataType::UInt16)
    {
        return DaneJoe::to_string(reinterpret_cast<const uint16_t*>(field.value.data()));
    }
    else if (field.type == DataType::Int16)
    {
        return DaneJoe::to_string(reinterpret_cast<const int16_t*>(field.value.data()));
    }
    else if (field.type == DataType::UInt32)
    {
        return DaneJoe::to_string(reinterpret_cast<const uint32_t*>(field.value.data()));
    }
    else if (field.type == DataType::Int32)
    {
        return DaneJoe::to_string(reinterpret_cast<const int32_t*>(field.value.data()));
    }
    else if (field.type == DataType::UInt64)
    {
        return DaneJoe::to_string(reinterpret_cast<const uint64_t*>(field.value.data()));
    }
    else if (field.type == DataType::Int64)
    {
        return DaneJoe::to_string(reinterpret_cast<const int64_t*>(field.value.data()));
    }
    else if (field.type == DataType::Float)
    {
        return DaneJoe::to_string(reinterpret_cast<const float*>(field.value.data()));
    }
    else if (field.type == DataType::Double)
    {
        return DaneJoe::to_string(reinterpret_cast<const double*>(field.value.data()));
    }
    else if (field.type == DataType::Bool)
    {
        return DaneJoe::to_string(reinterpret_cast<const bool*>(field.value.data()));
    }
    else
    {
        // 不支持直接字符串化的类型，打印出枚举类型信息
        return DaneJoe::to_string(field.type);
    }
}

std::vector<uint8_t>  DaneJoe::to_dictionary(const DaneJoe::DaneJoeSerializer::Field& field)
{
    if (field.type != DataType::Dictionary)
    {
        return std::vector<uint8_t>();
    }
    return field.value;
}

std::string DaneJoe::to_string(DaneJoe::DaneJoeSerializer::FieldFlag flag)
{
    bool is_valid = false;
    std::string flag_str;
    if(has_flag(flag, DaneJoe::DaneJoeSerializer::FieldFlag::HasValueLength))
    {
        flag_str += "HasValueLength|";
        is_valid = true;
    }
    if(is_valid)
    {
        flag_str.pop_back();
    }
    else
    {
        flag_str = "None";
    }
    return flag_str;
}

std::string DaneJoe::to_string(DaneJoe::DaneJoeSerializer::ArrayFlag flag)
{
    bool is_valid = false;
    std::string flag_str;
    if(has_flag(flag, DaneJoe::DaneJoeSerializer::ArrayFlag::IsElementLengthVariable))
    {
        flag_str += "IsElementLengthVariable|";
        is_valid = true;
    }
    if(is_valid)
    {
        flag_str.pop_back();
    }
    else
    {
        flag_str = "None";
    }
    return flag_str;
}

std::string DaneJoe::to_string(DaneJoe::DaneJoeSerializer::MapFlag flag)
{
    bool is_valid = false;
    std::string flag_str;
    if(has_flag(flag, DaneJoe::DaneJoeSerializer::MapFlag::IsKeyLengthVariable))
    {
        flag_str += "IsKeyLengthVariable|";
        is_valid = true;
    }
    if(has_flag(flag, DaneJoe::DaneJoeSerializer::MapFlag::IsValueLengthVariable))
    {
        flag_str += "IsValueLengthVariable|";
        is_valid = true;
    }
    if(is_valid)
    {
        flag_str.pop_back();
    }
    else
    {
        flag_str = "None";
    }
    return flag_str;
}

std::string DaneJoe::to_string(DaneJoe::DaneJoeSerializer::MessageFlag flag)
{
    bool is_valid = false;
    std::string flag_str;
    if(has_flag(flag, DaneJoe::DaneJoeSerializer::MessageFlag::HasCheckSum))
    {
        flag_str += "HasCheckSum|";
        is_valid = true;
    }
    if(is_valid)
    {
        flag_str.pop_back();
    }
    else
    {
        flag_str = "None";
    }
    return flag_str;
}
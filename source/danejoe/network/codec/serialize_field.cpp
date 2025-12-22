#include <format>

#include "danejoe/common/binary/byte_order.hpp"
#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/core/memory_util.hpp"
#include "danejoe/network/codec/serialize_field.hpp"
#include "danejoe/common/enum/enum_convert.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"

std::string DaneJoe::to_string(DaneJoe::SerializeFieldFlag flag)
{
    if (flag == SerializeFieldFlag::None)
    {
        return ENUM_TO_STRING(SerializeFieldFlag::None);
    }
    if (flag == SerializeFieldFlag::HasValueLength)
    {
        return ENUM_TO_STRING(SerializeFieldFlag::HasValueLength);
    }
    std::string result;
    if (has_flag(flag, SerializeFieldFlag::HasValueLength))
    {
        result += ENUM_TO_STRING(SerializeFieldFlag::HasValueLength);
        result += "|";
    }
    if (!result.empty())
    {
        result.pop_back();
        return result;
    }
    return ENUM_TO_STRING(SerializeFieldFlag::None);
}

template<>
DaneJoe::SerializeFieldFlag DaneJoe::enum_cast<DaneJoe::SerializeFieldFlag>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(SerializeFieldFlag::None))
    {
        return SerializeFieldFlag::None;
    }
    else if (enum_string == ENUM_TO_STRING(SerializeFieldFlag::HasValueLength))
    {
        return SerializeFieldFlag::HasValueLength;
    }
    return SerializeFieldFlag::None;
}

uint32_t DaneJoe::SerializeField::min_serialized_byte_array_size()
{
    uint32_t size
        = sizeof(name_length)// 名称长度所占字节
        + sizeof(type)// 数据类型所占字节
        + sizeof(flag);// 标志所占字节
    return size;
}

uint32_t DaneJoe::SerializeField::serialized_size()const
{
    uint32_t size
        = sizeof(name_length)// 名称长度所占字节
        + name.size()// 名称所占字节
        + sizeof(DataType)// 数据类型所占字节
        + sizeof(SerializeFieldFlag)// 标志所占字节
        + value.size();// 值所占字节
    if (has_flag(flag, SerializeFieldFlag::HasValueLength))
    {
        size += sizeof(value_length);// 值长度所占字节
    }
    return size;
}

std::vector<uint8_t> DaneJoe::SerializeField::get_value()const
{
    return value;
}

std::optional<DaneJoe::SerializeField> DaneJoe::SerializeField::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    uint32_t data_size = data.size();
    if (data_size < min_serialized_byte_array_size())
    {
        ADD_DIAG_WARN("network", "Deserialize field failed: data size {} is less than minimum field size {}", data_size, min_serialized_byte_array_size());
        return std::nullopt;
    }
    SerializeField field;
    uint32_t current_index = 0;
    uint32_t current_need_to_read_size = 0;
    // 获取名称长度字段
    current_need_to_read_size += sizeof(name_length);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read name_length, needed {}, available {}", current_need_to_read_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.name_length)), reinterpret_cast<const uint16_t*>(data.data() + current_index));
    current_index += sizeof(name_length);
    // 获取名称字段
    current_need_to_read_size += field.name_length;
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read name, needed {}, available {}", current_need_to_read_size, data.size());
        return std::nullopt;
    }
    field.name = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + field.name_length);
    current_index += field.name_length;
    // 获取数据类型字段
    current_need_to_read_size += sizeof(type);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read type, needed {}, available {}", current_need_to_read_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.type)), reinterpret_cast<const DataType*>(data.data() + current_index));
    current_index += sizeof(type);
    // 获取标志字段
    current_need_to_read_size += sizeof(flag);
    if (!ensure_enough_to_read(data, current_need_to_read_size))
    {
        ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read flag, needed {}, available {}", current_need_to_read_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(field.flag)), reinterpret_cast<const SerializeFieldFlag*>(data.data() + current_index));
    current_index += sizeof(flag);
    // 获取值长度字段
    if (has_flag<SerializeFieldFlag>(field.flag, SerializeFieldFlag::HasValueLength))
    {
        current_need_to_read_size += sizeof(value_length);
        if (!ensure_enough_to_read(data, current_need_to_read_size))
        {
            ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read value_length, needed {}, available {}", current_need_to_read_size, data.size());
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
        ADD_DIAG_WARN("network", "Deserialize field failed: not enough data to read value, needed {}, available {}", current_need_to_read_size, data.size());
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

std::vector<uint8_t> DaneJoe::SerializeField::to_serialized_byte_array()const
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
    if (has_flag(flag, SerializeFieldFlag::HasValueLength))
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

std::string DaneJoe::SerializeField::to_string()const
{
    std::string name_str(name.begin(), name.end());
    return std::format(
        "name_length={} | name={} | type={} | flag={} | value_length={} | value_size={}",
        name_length,
        name_str,
        DaneJoe::to_string(type),
        DaneJoe::to_string(flag),
        value_length,
        value.size());
}

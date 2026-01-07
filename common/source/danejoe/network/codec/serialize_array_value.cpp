#include <format>

#include "danejoe/common/binary/byte_order.hpp"
#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/core/memory_util.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/network/codec/serialize_array_value.hpp"

template<>
DaneJoe::SerializeArrayFlag DaneJoe::enum_cast<DaneJoe::SerializeArrayFlag>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(SerializeArrayFlag::None))
    {
        return SerializeArrayFlag::None;
    }
    else if (enum_string == ENUM_TO_STRING(SerializeArrayFlag::IsElementLengthVariable))
    {
        return SerializeArrayFlag::IsElementLengthVariable;
    }
    return SerializeArrayFlag::None;
}

std::string DaneJoe::to_string(DaneJoe::SerializeArrayFlag flag)
{
    if (flag == SerializeArrayFlag::None)
    {
        return ENUM_TO_STRING(SerializeArrayFlag::None);
    }
    if (flag == SerializeArrayFlag::IsElementLengthVariable)
    {
        return ENUM_TO_STRING(SerializeArrayFlag::IsElementLengthVariable);
    }
    std::string result;
    if (has_flag(flag, SerializeArrayFlag::IsElementLengthVariable))
    {
        result += ENUM_TO_STRING(SerializeArrayFlag::IsElementLengthVariable);
        result += "|";
    }
    if (!result.empty())
    {
        result.pop_back();
        return result;
    }
    return ENUM_TO_STRING(SerializeArrayFlag::None);
}

uint32_t DaneJoe::SerializeArrayValue::min_serialized_byte_array_size()
{
    uint32_t size
        = sizeof(element_type)// 数据类型所占字节
        + sizeof(element_count)// 元素数量所占字节
        + sizeof(flag)// 标志所占字节
        + sizeof(uint32_t);// 元素长度列表所占字节
    return size;
}

uint32_t DaneJoe::SerializeArrayValue::serialized_size()const
{
    uint32_t size
        = sizeof(element_type)// 数据类型所占字节
        + sizeof(element_count)// 元素数量所占字节
        + sizeof(flag)// 标志所占字节
        + sizeof(uint32_t) * element_value_length.size()// 元素长度列表所占字节
        + sizeof(uint8_t) * element_value.size();// 元素所占字节
    return size;
}

std::optional<DaneJoe::SerializeArrayValue> DaneJoe::SerializeArrayValue::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    if (data.size() < min_serialized_byte_array_size())
    {
        ADD_DIAG_WARN("network", "Deserialize array failed: data size {} is less than minimum array size {}", data.size(), min_serialized_byte_array_size());
        return std::nullopt;
    }
    SerializeArrayValue array_value;
    uint32_t current_index = 0;
    uint32_t current_need_size = 0;
    // 获取数据类型字段
    current_need_size += sizeof(element_type);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read element_type, needed {}, available {}", current_need_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.element_type)), reinterpret_cast<const DataType*>(data.data() + current_index));
    current_index += sizeof(element_type);
    // 获取元素数量字段
    current_need_size += sizeof(element_count);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read element_count, needed {}, available {}", current_need_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.element_count)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(element_count);
    // 获取标志字段
    current_need_size += sizeof(flag);
    if (!ensure_enough_to_read(data, current_need_size))
    {
        ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read flag, needed {}, available {}", current_need_size, data.size());
        return std::nullopt;
    }
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(array_value.flag)), reinterpret_cast<const SerializeArrayFlag*>(data.data() + current_index));
    current_index += sizeof(flag);
    uint32_t value_length = 0;
    // 检查数组元素长度是否可变
    if (has_flag<SerializeArrayFlag>(array_value.flag, SerializeArrayFlag::IsElementLengthVariable))
    {
        // 写入元素长度列表
        array_value.element_value_length.resize(array_value.element_count);
        // 写入每个元素长度
        for (uint32_t i = 0; i < array_value.element_count; ++i)
        {
            current_need_size += sizeof(uint32_t);
            if (!ensure_enough_to_read(data, current_need_size))
            {
                ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read element_value_length[{}], needed {}, available {}", i, current_need_size, data.size());
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
            ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read element_value_length, needed {}, available {}", current_need_size, data.size());
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
        ADD_DIAG_WARN("network", "Deserialize array failed: not enough data to read element_value, needed {}, available {}", current_need_size, data.size());
        return std::nullopt;
    }
    array_value.element_value = std::vector<uint8_t>(data.begin() + current_index, data.begin() + current_index + value_length);
    current_index += value_length;
    return array_value;
}

std::vector<uint8_t> DaneJoe::SerializeArrayValue::to_serialized_byte_array()const
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
        ADD_DIAG_ERROR("network", "Serialize array failed: element_count does not match element_value_length size");
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

std::string DaneJoe::SerializeArrayValue::to_string()const
{
    return std::format(
        "element_type={} | element_count={} | flag={} | element_value_length_size={} | element_value_size={}",
        DaneJoe::to_string(element_type),
        element_count,
        DaneJoe::to_string(flag),
        element_value_length.size(),
        element_value.size());
}

#include <format>

#include "danejoe/network/codec/serialize_header.hpp"
#include "danejoe/common/binary/byte_order.hpp"
#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"

std::string DaneJoe::to_string(DaneJoe::SerializeFlag flag)
{
    if (flag == SerializeFlag::None)
    {
        return ENUM_TO_STRING(SerializeFlag::None);
    }
    if (flag == SerializeFlag::HasCheckSum)
    {
        return ENUM_TO_STRING(SerializeFlag::HasCheckSum);
    }
    std::string result;
    if (has_flag(flag, SerializeFlag::HasCheckSum))
    {
        result += ENUM_TO_STRING(SerializeFlag::HasCheckSum);
        result += "|";
    }
    if (!result.empty())
    {
        result.pop_back();
        return result;
    }
    return ENUM_TO_STRING(SerializeFlag::None);
}

template<>
DaneJoe::SerializeFlag DaneJoe::enum_cast<DaneJoe::SerializeFlag>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(SerializeFlag::None))
    {
        return SerializeFlag::None;
    }
    else if (enum_string == ENUM_TO_STRING(SerializeFlag::HasCheckSum))
    {
        return SerializeFlag::HasCheckSum;
    }
    return SerializeFlag::None;
}

uint32_t DaneJoe::SerializeHeader::min_serialized_byte_array_size()
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

uint32_t DaneJoe::SerializeHeader::serialized_size()const
{
    // 序列化长度与最小内容所占长度一致（无变长部分）
    return min_serialized_byte_array_size();
}

std::optional<DaneJoe::SerializeHeader> DaneJoe::SerializeHeader::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    SerializeHeader header;
    // 检查源长度是否合法
    if (data.size() < min_serialized_byte_array_size())
    {
        ADD_DIAG_WARN("network", "Deserialize header failed: data size {} is less than minimum header size {}", data.size(), min_serialized_byte_array_size());
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
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.flag)), reinterpret_cast<const SerializeFlag*>(data.data() + current_index));
    current_index += sizeof(SerializeFlag);
    // 获取校验码部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.checksum)), reinterpret_cast<const uint32_t*>(data.data() + current_index));
    current_index += sizeof(checksum);
    // 获取块数量部分，转换字节序，并写入结构体
    to_local_byte_order(reinterpret_cast<uint8_t*>(&(header.field_count)), reinterpret_cast<const uint16_t*>(data.data() + current_index));
    current_index += sizeof(field_count);
    return header;
}

std::vector<uint8_t> DaneJoe::SerializeHeader::to_serialized_byte_array()const
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

std::string DaneJoe::SerializeHeader::to_string()const
{
    return std::format(
        "magic_number={} | version={} | message_length={} | flag={} | checksum={} | field_count={}",
        magic_number,
        static_cast<int>(version),
        message_length,
        DaneJoe::to_string(flag),
        checksum,
        field_count);
}

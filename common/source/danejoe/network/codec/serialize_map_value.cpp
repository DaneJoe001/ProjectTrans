#include <format>

#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/core/variable_util.hpp"
#include "danejoe/network/codec/serialize_map_value.hpp"

template<>
DaneJoe::SerializeMapFlag DaneJoe::enum_cast<DaneJoe::SerializeMapFlag>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(SerializeMapFlag::None))
    {
        return SerializeMapFlag::None;
    }
    else if (enum_string == ENUM_TO_STRING(SerializeMapFlag::IsKeyLengthVariable))
    {
        return SerializeMapFlag::IsKeyLengthVariable;
    }
    else if (enum_string == ENUM_TO_STRING(SerializeMapFlag::IsValueLengthVariable))
    {
        return SerializeMapFlag::IsValueLengthVariable;
    }
    return SerializeMapFlag::None;
}

std::string DaneJoe::to_string(DaneJoe::SerializeMapFlag flag)
{
    if (flag == SerializeMapFlag::None)
    {
        return ENUM_TO_STRING(SerializeMapFlag::None);
    }
    if (flag == SerializeMapFlag::IsKeyLengthVariable)
    {
        return ENUM_TO_STRING(SerializeMapFlag::IsKeyLengthVariable);
    }
    if (flag == SerializeMapFlag::IsValueLengthVariable)
    {
        return ENUM_TO_STRING(SerializeMapFlag::IsValueLengthVariable);
    }
    std::string result;
    if (has_flag(flag, SerializeMapFlag::IsKeyLengthVariable))
    {
        result += ENUM_TO_STRING(SerializeMapFlag::IsKeyLengthVariable);
        result += "|";
    }
    if (has_flag(flag, SerializeMapFlag::IsValueLengthVariable))
    {
        result += ENUM_TO_STRING(SerializeMapFlag::IsValueLengthVariable);
        result += "|";
    }
    if (!result.empty())
    {
        result.pop_back();
        return result;
    }
    return ENUM_TO_STRING(SerializeMapFlag::None);
}

uint32_t DaneJoe::SerializeMapValue::serialized_size()const
{
    /// @todo 当前不做实现，后续再完善
    uint32_t size = 0;
    return size;
}

std::optional<DaneJoe::SerializeMapValue> DaneJoe::SerializeMapValue::from_serialized_byte_array(const std::vector<uint8_t>& data)
{
    /// @todo 后续实现
    DANEJOE_UNUSED(data)
    return std::nullopt;
}

std::vector<uint8_t> DaneJoe::SerializeMapValue::to_serialized_byte_array()const
{
    /// @todo 当前不做实现，后续再完善
    return std::vector<uint8_t>();
}

std::string DaneJoe::SerializeMapValue::to_string()const
{
    return std::format(
        "key_type={} | value_type={} | element_count={} | flag={} | key_length_size={} | value_length_size={} | key_value_size={} | value_value_size={}",
        DaneJoe::to_string(key_type),
        DaneJoe::to_string(value_type),
        element_count,
        DaneJoe::to_string(flag),
        key_length.size(),
        value_length.size(),
        key_value.size(),
        value_value.size());
}

#include "danejoe/common/binary/byte_order.hpp"

std::string DaneJoe::to_string(ByteOrder order)
{
    switch (order)
    {
    case ByteOrder::LittleEndian:
        return ENUM_TO_STRING(ByteOrder::LittleEndian);
    case ByteOrder::BigEndian:
        return ENUM_TO_STRING(ByteOrder::BigEndian);
    case ByteOrder::Unknown:
    default:
        return ENUM_TO_STRING(ByteOrder::Unknown);
    }
}

template<>
DaneJoe::ByteOrder DaneJoe::enum_cast<DaneJoe::ByteOrder>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(ByteOrder::LittleEndian))
    {
        return ByteOrder::LittleEndian;
    }
    else if (enum_string == ENUM_TO_STRING(ByteOrder::BigEndian))
    {
        return ByteOrder::BigEndian;
    }
    else if (enum_string == ENUM_TO_STRING(ByteOrder::Unknown))
    {
        return ByteOrder::Unknown;
    }
    return ByteOrder::Unknown;
}

DaneJoe::ByteOrder DaneJoe::get_local_byte_order()noexcept
{
    static bool is_get_byte_order = false;
    static DaneJoe::ByteOrder byte_order = DaneJoe::ByteOrder::LittleEndian;
    if (!is_get_byte_order)
    {
        uint16_t data = 1;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&data);
        if (ptr[0] == 1)
        {
            // 低字节在低地址
            byte_order = DaneJoe::ByteOrder::LittleEndian;
        }
        else
        {
            // 低字节在高地址
            byte_order = DaneJoe::ByteOrder::BigEndian;
        }
        is_get_byte_order = true;
    }
    return byte_order;
}

bool DaneJoe::is_network_byte_order()
{
    static bool is_init = false;
    static bool is_local_byte_order_same_to_network_flag = false;
    if (!is_init)
    {
        auto local_byte_order = get_local_byte_order();
        is_local_byte_order_same_to_network_flag = local_byte_order == DANEJOE_NETWORK_BYTE_ORDER;
        is_init = true;
    }
    return is_local_byte_order_same_to_network_flag;
}


std::vector<uint8_t> DaneJoe::to_local_byte_order(const std::vector<uint8_t>& data)
{
    std::size_t size = data.size();
    std::vector<uint8_t> result(size);
    to_local_byte_order<uint8_t>(result.data(), data.data(), size);
    return result;
}

std::vector<uint8_t> DaneJoe::to_network_byte_order(const std::vector<uint8_t>& data)
{
    std::size_t size = data.size();
    std::vector<uint8_t> result(size);
    to_network_byte_order<uint8_t>(result.data(), data.data(), size);
    return result;
}

std::vector<uint8_t> DaneJoe::to_network_byte_order(const uint8_t* data, uint32_t count)
{
    std::vector<uint8_t> result(count);
    convert_byte_order(result.data(), data, count);
    return result;
}

std::vector<uint8_t> DaneJoe::to_local_byte_order(const uint8_t* data, uint32_t count)
{
    std::vector<uint8_t> result(count);
    convert_byte_order(result.data(), data, count);
    return result;
}

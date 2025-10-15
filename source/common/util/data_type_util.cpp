#include "common/util/data_type_util.hpp"

uint32_t DaneJoe::get_data_type_length(DataType type)
{
    switch (type)
    {
    case DataType::UInt8:
    case DataType::Int8:
        return 1;
    case DataType::UInt16:
    case DataType::Int16:
        return 2;
    case DataType::UInt32:
    case DataType::Int32:
    case DataType::Float:
        return 4;
    case DataType::UInt64:
    case DataType::Int64:
    case DataType::Double:
        return 8;
    case DataType::Bool:
        return 1;
    default:
        /// @todo 待完善
        return 0;
    }
}

std::string DaneJoe::to_string(DataType type)
{
    switch (type)
    {
    case DataType::Null:
        return "Null";
    case DataType::UInt8:
        return "UInt8";
    case DataType::Int8:
        return "Int8";
    case DataType::UInt16:
        return "UInt16";
    case DataType::Int16:
        return "Int16";
    case DataType::UInt32:
        return "UInt32";
    case DataType::Int32:
        return "Int32";
    case DataType::UInt64:
        return "UInt64";
    case DataType::Int64:
        return "Int64";
    case DataType::Float:
        return "Float";
    case DataType::Double:
        return "Double";
    case DataType::Bool:
        return "Bool";
    case DataType::String:
        return "String";
    case DataType::ByteArray:
        return "ByteArray";
    case DataType::Array:
        return "Array";
    case DataType::Dictionary:
        return "Dictionary";
    default:
        return "Unknown";
    }
}
DaneJoe::DataType DaneJoe::to_data_type(uint8_t type_code)
{
    return static_cast<DataType>(type_code);
}
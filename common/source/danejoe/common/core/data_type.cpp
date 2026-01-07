#include "danejoe/common/core/data_type.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

uint32_t DaneJoe::get_data_type_length(DataType type)
{
    switch (type)
    {
    case DataType::UInt8:  return 1;
    case DataType::Int8:   return 1;
    case DataType::UInt16: return 2;
    case DataType::Int16:  return 2;
    case DataType::UInt32: return 4;
    case DataType::Int32:  return 4;
    case DataType::UInt64: return 8;
    case DataType::Int64:  return 8;
    case DataType::Float:  return 4;
    case DataType::Double: return 8;
    default: return 0;
    }
}

std::string DaneJoe::to_string(DataType type)
{
    switch (type)
    {
    case DataType::Object:
        return ENUM_TO_STRING(DataType::Object);
    case DataType::UInt8:
        return ENUM_TO_STRING(DataType::UInt8);
    case DataType::Int8:
        return ENUM_TO_STRING(DataType::Int8);
    case DataType::UInt16:
        return ENUM_TO_STRING(DataType::UInt16);
    case DataType::Int16:
        return ENUM_TO_STRING(DataType::Int16);
    case DataType::UInt32:
        return ENUM_TO_STRING(DataType::UInt32);
    case DataType::Int32:
        return ENUM_TO_STRING(DataType::Int32);
    case DataType::UInt64:
        return ENUM_TO_STRING(DataType::UInt64);
    case DataType::Int64:
        return ENUM_TO_STRING(DataType::Int64);
    case DataType::Float:
        return ENUM_TO_STRING(DataType::Float);
    case DataType::Double:
        return ENUM_TO_STRING(DataType::Double);
    case DataType::Bool:
        return ENUM_TO_STRING(DataType::Bool);
    case DataType::String:
        return ENUM_TO_STRING(DataType::String);
    case DataType::ByteArray:
        return ENUM_TO_STRING(DataType::ByteArray);
    case DataType::Map:
        return ENUM_TO_STRING(DataType::Map);
    case DataType::Array:
        return ENUM_TO_STRING(DataType::Array);
    case DataType::Dictionary:
        return ENUM_TO_STRING(DataType::Dictionary);
    case DataType::Null:
        return ENUM_TO_STRING(DataType::Null);
    case DataType::Unknown:
        return ENUM_TO_STRING(DataType::Unknown);
    default:
        return ENUM_TO_STRING(DataType::Unknown);
    }
}

template<>
DaneJoe::DataType DaneJoe::enum_cast<DaneJoe::DataType>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(DataType::Object))
    {
        return DataType::Object;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::UInt8))
    {
        return DataType::UInt8;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Int8))
    {
        return DataType::Int8;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::UInt16))
    {
        return DataType::UInt16;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Int16))
    {
        return DataType::Int16;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::UInt32))
    {
        return DataType::UInt32;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Int32))
    {
        return DataType::Int32;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::UInt64))
    {
        return DataType::UInt64;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Int64))
    {
        return DataType::Int64;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Float))
    {
        return DataType::Float;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Double))
    {
        return DataType::Double;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Bool))
    {
        return DataType::Bool;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::String))
    {
        return DataType::String;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::ByteArray))
    {
        return DataType::ByteArray;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Map))
    {
        return DataType::Map;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Array))
    {
        return DataType::Array;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Dictionary))
    {
        return DataType::Dictionary;
    }
    else if (enum_string == ENUM_TO_STRING(DataType::Null))
    {
        return DataType::Null;
    }
    return DataType::Unknown;
}
template<>
DaneJoe::DataType DaneJoe::enum_cast<DaneJoe::DataType>(int64_t enum_value)
{
    // 若外部有自定义编码规则，可在此调整映射。
    // 默认按枚举顺序进行映射，越界则返回 Unknown。
    switch (enum_value)
    {
    case 0:  return DataType::Object;
    case 1:  return DataType::UInt8;
    case 2:  return DataType::Int8;
    case 3:  return DataType::UInt16;
    case 4:  return DataType::Int16;
    case 5:  return DataType::UInt32;
    case 6:  return DataType::Int32;
    case 7:  return DataType::UInt64;
    case 8:  return DataType::Int64;
    case 9:  return DataType::Float;
    case 10: return DataType::Double;
    case 11: return DataType::Bool;
    case 12: return DataType::String;
    case 13: return DataType::ByteArray;
    case 14: return DataType::Map;
    case 15: return DataType::Array;
    case 16: return DataType::Dictionary;
    case 17: return DataType::Null;
    default: return DataType::Unknown;
    }
}

bool DaneJoe::is_integer_type(DataType type)
{
    switch (type)
    {
    case DaneJoe::DataType::UInt8:
    case DaneJoe::DataType::Int8:
    case DaneJoe::DataType::UInt16:
    case DaneJoe::DataType::Int16:
    case DaneJoe::DataType::UInt32:
    case DaneJoe::DataType::Int32:
    case DaneJoe::DataType::UInt64:
    case DaneJoe::DataType::Int64:
        return true;
    default:
        return false;
    }
}

bool DaneJoe::is_string_type(DataType type)
{
    return type == DaneJoe::DataType::String;
}

bool DaneJoe::is_float_type(DataType type)
{
    switch (type)
    {
    case DaneJoe::DataType::Float:
    case DaneJoe::DataType::Double:
        return true;
    default:
        return false;
    }
}

bool DaneJoe::is_bool_type(DataType type)
{
    return type == DaneJoe::DataType::Bool;
}

bool DaneJoe::is_null_type(DataType type)
{
    return type == DaneJoe::DataType::Null;
}

bool DaneJoe::is_unknown_type(DataType type)
{
    return type == DaneJoe::DataType::Unknown;
}

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
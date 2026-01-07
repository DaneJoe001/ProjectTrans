#include "danejoe/common/binary/byte_order.hpp"
#include "danejoe/common/enum/enum_flag.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/common/core/variable_util.hpp"
#include "danejoe/stringify/stringify_to_string.hpp"
#include "danejoe/network/codec/serialize_codec.hpp"

const uint32_t DaneJoe::SerializeCodec::HEADER_SIZE = 16;

DaneJoe::SerializeConfig::SerializeConfig()
{}

uint16_t DaneJoe::SerializeCodec::get_message_header_size()
{
    return HEADER_SIZE;
}

DaneJoe::SerializeCodec::SerializeCodec(const SerializeConfig& config) :m_serialized_config(config), m_serialized_byte_array_build(std::vector<uint8_t>(config.pre_allocated_size))
{
    ADD_DIAG_TRACE("network", "Serialized config: max_field_value_length={}, max_field_name_length={}, pre_allocated_size={}",
        config.max_field_value_length, config.max_field_name_length, config.pre_allocated_size);
}

void DaneJoe::SerializeCodec::set_config(const SerializeConfig& config)
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
void DaneJoe::SerializeCodec::reset_build()
{
    m_serialized_byte_array_build.clear();
    m_serialized_data_map_build.clear();
    m_current_index = HEADER_SIZE;
}

void DaneJoe::SerializeCodec::reset_parse()
{
    m_serialized_byte_array_parsed.clear();
    m_serialized_data_map_parsed.clear();
    m_parsed_buffer_index = 0;
    m_is_parsed_header_finished = false;
}

std::vector<uint8_t>  DaneJoe::SerializeCodec::get_serialized_data_vector_build()noexcept
{
    finalize_message_header();
    return m_serialized_byte_array_build;
}

std::vector<uint8_t> DaneJoe::SerializeCodec::get_serialized_data_vector_parsed()const noexcept
{
    return m_serialized_byte_array_parsed;
}

void DaneJoe::SerializeCodec::deserialize(const std::vector<uint8_t>& data)
{
    auto header_optional = get_message_header(data);
    if (!header_optional.has_value())
    {
        ADD_DIAG_WARN("network", "Deserialize message skipped: header parse failed");
        return;
    }
    // 消息长度超出数据长度
    auto header = header_optional.value();
    if (header.message_length + HEADER_SIZE > data.size())
    {
        ADD_DIAG_WARN("network", "Deserialize message skipped: message length {} exceeds data size {}", header.message_length + HEADER_SIZE, data.size());
        return;
    }
    m_serialized_byte_array_parsed = data;
    uint32_t current_index = HEADER_SIZE;
    for (uint32_t i = 0; i < header.field_count; ++i)
    {
        SerializeField field;
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
        if (has_flag(field.flag, SerializeFieldFlag::HasValueLength))
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

void DaneJoe::SerializeCodec::deserialize(const std::string& data)
{
    std::vector<uint8_t> data_vector(data.begin(), data.end());
    deserialize(data_vector);
}

void DaneJoe::SerializeCodec::deserialize()
{

}

DaneJoe::SerializeCodec& DaneJoe::SerializeCodec::serialize(const SerializeField& field)
{
    if (field.name_length > m_serialized_config.max_field_name_length)
    {
        ADD_DIAG_WARN("network", "Serialize field skipped: name length {} exceeds maximum limit {}", field.name_length, m_serialized_config.max_field_name_length);
        return *this;
    }
    if (field.value_length > m_serialized_config.max_field_value_length)
    {
        ADD_DIAG_WARN("network", "Serialize field skipped: value length {} exceeds maximum limit {}", field.value_length, m_serialized_config.max_field_value_length);
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

void DaneJoe::SerializeCodec::finalize_message_header()
{
    m_serialized_byte_array_build.resize(m_current_index);
    SerializeHeader header;
    header.message_length = m_current_index - HEADER_SIZE;
    header.flag = SerializeFlag::None;
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

std::optional<DaneJoe::SerializeHeader> DaneJoe::SerializeCodec::get_message_header(const std::vector<uint8_t>& data)const noexcept
{
    if (data.size() < HEADER_SIZE)
    {
        ADD_DIAG_WARN("network", "Deserialize header failed: data size {} is less than header size {}", data.size(), get_message_header_size());
        return std::nullopt;
    }
    auto header_optional = DaneJoe::SerializeHeader::from_serialized_byte_array(std::vector<uint8_t>(data.begin(), data.begin() + HEADER_SIZE));
    return header_optional;
}

void DaneJoe::SerializeCodec::ensure_enough_capacity_rest_to_build(uint32_t size_need)
{
    auto size = m_serialized_byte_array_build.size();
    auto request_total_size = m_current_index + size_need;
    if (request_total_size > size)
    {
        m_serialized_byte_array_build.resize(request_total_size);
    }
}

void DaneJoe::SerializeCodec::add_data_block_to_parse(const std::vector<uint8_t>& data)
{
    // 先判断是否有足够的空间容量
    uint32_t size_need = m_parsed_buffer_index + data.size();
    if (size_need > m_serialized_byte_array_parsed.size())
    {
        m_serialized_byte_array_parsed.resize(size_need);
    }
    for (const auto& byte : data)
    {
        m_serialized_byte_array_parsed[m_parsed_buffer_index++] = byte;
    }
}

bool DaneJoe::SerializeCodec::is_frame_complete()
{
    if (m_parsed_buffer_index < HEADER_SIZE)
    {
        ADD_DIAG_DEBUG("network", "Check frame skipped: buffer index {} is less than header size {}", m_parsed_buffer_index, HEADER_SIZE);
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

DaneJoe::SerializeCodec& DaneJoe::SerializeCodec::serialize(const std::string& data_name, const ISerializeDictionary& data)
{
    /// @todo 后续再完善
    DANEJOE_UNUSED(data_name)
    DANEJOE_UNUSED(data)
    return *this;
}

std::optional<DaneJoe::SerializeField> DaneJoe::SerializeCodec::get_parsed_field(const std::string& key)const
{
    auto field_it = m_serialized_data_map_parsed.find(key);
    if (field_it == m_serialized_data_map_parsed.end())
    {
        ADD_DIAG_INFO("network", "Get parsed field skipped: key '{}' not found", key);
        return std::nullopt;
    }
    return field_it->second;
}

std::unordered_multimap<std::string, DaneJoe::SerializeField> DaneJoe::SerializeCodec::get_parsed_data_map()const noexcept
{
    return m_serialized_data_map_parsed;
}

std::string DaneJoe::to_string(const DaneJoe::SerializeField& field)
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

std::vector<uint8_t>  DaneJoe::to_dictionary(const DaneJoe::SerializeField& field)
{
    if (field.type != DataType::Dictionary)
    {
        return std::vector<uint8_t>();
    }
    return field.value;
}


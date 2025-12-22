#include "danejoe/network/codec/serialize_codec.hpp"
#include "danejoe/network/codec/serialize_header.hpp"

#include "common/protocol/frame_assembler.hpp"

void DaneJoe::ensure_capacity(std::vector<uint8_t>& vec, size_t size)
{
    /// @todo 考虑抛出异常
    if (vec.size() > size)
    {
        return;
    }
    vec.resize(size);
}

void FrameAssembler::push_data(const std::vector<uint8_t>& data)
{
    for(const auto& byte : data)
    {
        m_buffer.push_back(byte);
    }
}

std::vector<uint8_t> FrameAssembler::pop_data(uint32_t size)
{
    std::vector<uint8_t> data;
    for (uint32_t i = 0; i < size && !m_buffer.empty(); ++i)
    {
        data.push_back(m_buffer.front());
        m_buffer.pop_front();
    }
    return data;
}


std::optional<std::vector<uint8_t>> FrameAssembler::pop_frame()
{
    auto header_size = DaneJoe::SerializeCodec::get_message_header_size();
    // 检查是否以获取当前消息头
    if (!m_is_got_header)
    {
        // 检查是否有足够的数据解析消息头
        if (m_buffer.size() < header_size)
        {
            return std::nullopt;
        }
        auto header_data = pop_data(header_size);
        DaneJoe::ensure_capacity(m_current_frame, header_size);
        // 将消息头数据写入当前帧
        for (;m_current_frame_index < header_size; ++m_current_frame_index)
        {
            m_current_frame[m_current_frame_index] = header_data[m_current_frame_index];
        }
        auto header_opt = DaneJoe::SerializeHeader::from_serialized_byte_array(header_data);
        if (!header_opt.has_value())
        {
            clear_current_frame();
            return std::nullopt;
        }
        m_current_header = header_opt.value();
        m_is_got_header = true;
    }
    // 检查是否有足够长度的数据解析消息体
    if (m_buffer.size() < m_current_header.message_length)
    {
        return std::nullopt;
    }
    auto body_data = pop_data(m_current_header.message_length);
    // 确保当前帧有足够空间写入数据
    auto frame_size = header_size + m_current_header.message_length;
    DaneJoe::ensure_capacity(m_current_frame, frame_size);
    for (uint32_t i=0;m_current_frame_index < frame_size; ++m_current_frame_index,i++)
    {
        m_current_frame[m_current_frame_index] = body_data[i];
    }
    m_current_frame.resize(frame_size);
    clear_current_frame();
    return m_current_frame;
}

void FrameAssembler::clear_current_frame()
{
    m_current_frame_index = 0;
    m_is_got_header = false;
}
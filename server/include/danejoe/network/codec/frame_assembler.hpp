#pragma once

#include <deque>
#include <vector>
#include <optional>
#include <cstdint>

#include "danejoe/network/codec/serialize_header.hpp"

namespace DaneJoe
{
    class FrameAssembler
    {
    public:
        void push_data(const std::vector<uint8_t>& data);
        std::vector<uint8_t> pop_data(uint32_t size);
        /**
         * @brief 获取一个完整帧
         */
        std::optional<std::vector<uint8_t>> pop_frame();
        void clear_current_frame();
    private:
        /// @brief 接收缓冲区
        std::deque<uint8_t> m_buffer;
        std::vector<uint8_t> m_current_frame;
        uint32_t m_current_frame_index = 0;
        DaneJoe::SerializeHeader m_current_header;
        bool m_is_got_header = false;
    };
}
    
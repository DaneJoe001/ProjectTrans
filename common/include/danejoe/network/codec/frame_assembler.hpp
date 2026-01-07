/**
 * @file frame_assembler.hpp
 * @brief 帧组装器
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 FrameAssembler，用于将输入的字节流按 SerializeHeader 描述的协议组装为完整帧。
 */

#pragma once

#include <deque>
#include <vector>
#include <optional>
#include <cstdint>

#include "danejoe/network/codec/serialize_header.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @class FrameAssembler
     * @brief 帧组装器
     * @details 维护接收缓冲区并按协议解析帧头（SerializeHeader），随后累计帧体数据直到得到完整帧。
     */
    class FrameAssembler
    {
    public:
        /**
         * @brief 写入接收到的字节数据
         * @param data 新到达的数据片段
         */
        void push_data(const std::vector<uint8_t>& data);
        /**
         * @brief 从接收缓冲区弹出指定字节数
         * @param size 要弹出的字节数
         * @return 弹出的字节序列
         */
        std::vector<uint8_t> pop_data(uint32_t size);
        /**
         * @brief 获取一个完整帧
         * @return 完整帧数据；若当前缓冲区数据不足以组成完整帧则返回 std::nullopt
         */
        std::optional<std::vector<uint8_t>> pop_frame();
        /**
         * @brief 清理当前正在组装的帧状态
         */
        void clear_current_frame();
    private:
        /// @brief 接收缓冲区
        std::deque<uint8_t> m_buffer;
        /// @brief 当前帧缓存
        std::vector<uint8_t> m_current_frame;
        /// @brief 当前帧已接收字节数
        uint32_t m_current_frame_index = 0;
        /// @brief 当前帧头
        DaneJoe::SerializeHeader m_current_header;
        /// @brief 是否已解析到当前帧头
        bool m_is_got_header = false;
    };
}
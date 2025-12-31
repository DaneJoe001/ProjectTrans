/**
 * @file frame_assembler.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 对单条 TCP 字节流做组帧（从字节流中恢复出完整 frame）。
 * @date 2025-12-28
 */
#pragma once

#include <deque>
#include <vector>
#include <optional>
#include <cstdint>

#include "danejoe/network/codec/serialize_header.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @brief 确保缓冲区有足够的空间
     * @param vec 缓冲区
     * @param size 需要的空间大小
     */
    void ensure_capacity(std::vector<uint8_t>& vec, size_t size);
}

/**
 * @brief 组帧类
 */
class FrameAssembler
{
public:
    /**
     * @brief 添加数据到缓冲区
     * @param data 数据
     */
    void push_data(const std::vector<uint8_t>& data);
    /**
     * @brief 从缓冲区中弹出数据
     * @param size 需要弹出的数据大小
     * @return 弹出的数据
     */
    std::vector<uint8_t> pop_data(uint32_t size);
    /**
     * @brief 获取一个完整帧
     */
    std::optional<std::vector<uint8_t>> pop_frame();
    /**
     * @brief 清除当前帧
     */
    void clear_current_frame();
private:
    /// @brief 接收缓冲区
    std::deque<uint8_t> m_buffer;
    /// @brief 当前帧
    std::vector<uint8_t> m_current_frame;
    /// @brief 当前帧索引
    uint32_t m_current_frame_index = 0;
    /// @brief 当前帧头
    DaneJoe::SerializeHeader m_current_header;
    /// @brief 是否已获取当前帧头
    bool m_is_got_header = false;
};
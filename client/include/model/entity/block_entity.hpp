/**
 * @file block_entity.hpp
 * @brief 块实体
 * @author DaneJoe001
 */
#pragma once

#include <cstdint>
#include <chrono>
#include <string>

/**
 * @class BlockState
 * @brief 块状态
 */
enum class BlockState
{
    /// @brief 完成
    Completed,
    /// @brief 失败
    Failed,
    /// @brief 传输中
    InTransfer,
    /// @brief 等待
    Waiting,
    /// @brief 未知
    Unknown
};

std::string to_string(BlockState state);

/**
 * @struct BlockEntity
 * @brief 块信息实体
 */
struct BlockEntity
{
    /// @brief 请求块ID(由数据库自动生成)
    int64_t block_id = -1;
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 块偏移
    int64_t offset;
    /// @brief 块大小
    int64_t block_size;
    /// @brief 块状态
    BlockState state;
    /// @brief 开始时间
    std::chrono::time_point<std::chrono::system_clock> start_time;
    /// @brief 结束时间
    std::chrono::time_point<std::chrono::system_clock> end_time;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};
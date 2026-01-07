/**
 * @file block_transfer.hpp
 * @brief 块请求/响应传输模型
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @struct BlockRequestTransfer
 * @brief 网络块请求传输对象
 * @details 用于向服务端请求指定文件的某个数据块。
 */
struct BlockRequestTransfer
{
    /// @brief 请求块ID(由数据库自动生成)
    int64_t block_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 块偏移
    int64_t offset = 0;
    /// @brief 块大小
    int64_t block_size = 0;
    /**
     * @brief 转换为字符串
     * @return 字符串
     * @details 将块请求传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};

/**
 * @struct BlockResponseTransfer
 * @brief 网络块响应传输对象
 * @details 服务端对块请求的响应，包含块元信息与数据内容。
 */
struct BlockResponseTransfer
{
    /// @brief 请求块ID(由数据库自动生成)
    int64_t block_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 块偏移
    int64_t offset = 0;
    /// @brief 块大小
    int64_t block_size = 0;
    /// @brief 数据
    std::vector<uint8_t> data;
    /**
     * @brief 转换为字符串
     * @return 字符串
     * @details 将块响应传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};
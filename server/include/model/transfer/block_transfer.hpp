#pragma once

/**
 * @file block_request_info.hpp
 * @brief 请求块信息
 * @author DaneJoe001
 */

#include <cstdint>
#include <string>
#include <vector>

 /**
  * @class BlockRequestModel
  * @brief 网络块请求模型
  */
struct BlockRequestTransfer
{
    /// @brief 请求块ID(由数据库自动生成)
    int64_t block_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
    int64_t task_id = -1;
    /// @brief 块偏移
    int64_t offset = 0;
    /// @brief 块大小
    int64_t block_size = 0;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

/**
 * @class BlockResponseModel
 * @brief 网络块响应模型
 */
struct BlockResponseTransfer
{
    /// @brief 请求块ID(由数据库自动生成)
    int64_t block_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
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
     */
    std::string to_string() const;
};
/**
 * @file download_transfer.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 下载传输模型
 * @date 2026-01-06
 */

#pragma once

#include <cstdint>
#include <string>

/**
 * @struct DownloadRequestTransfer
 * @brief 下载请求传输模型
 */
struct DownloadRequestTransfer
{
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 任务ID
    int64_t task_id = -1;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

/**
 * @struct DownloadResponseTransfer
 * @brief 下载响应传输模型
 */
struct DownloadResponseTransfer
{
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 文件名称
    std::string file_name;
    /// @brief 文件大小
    int64_t file_size = 0;
    /// @brief MD5码
    std::string md5_code;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};
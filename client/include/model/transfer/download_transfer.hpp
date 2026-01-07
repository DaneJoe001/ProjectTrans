/**
  * @file download_transfer.hpp
  * @brief 下载请求/响应传输模型
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <cstdint>
#include <string>

/**
 * @struct DownloadRequestTransfer
 * @brief 下载请求传输对象
 * @details 用于向服务端发起下载请求。
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
     * @details 将下载请求传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};

/**
 * @struct DownloadResponseTransfer
 * @brief 下载响应传输对象
 * @details 服务端对下载请求的响应，包含文件元信息（名称、大小、校验信息等）。
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
     * @details 将下载响应传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};
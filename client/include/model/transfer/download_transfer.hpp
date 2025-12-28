#pragma once

#include <cstdint>
#include <string>

struct DownloadRequestTransfer
{
    /// @brief 文件ID
    int64_t file_id = -1;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

struct DownloadResponseTransfer
{
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 文件名称
    std::string saved_name;
    /// @brief 源路径
    std::string source_path;
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
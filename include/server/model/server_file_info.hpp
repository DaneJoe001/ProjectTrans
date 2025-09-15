#pragma once

/**
 * @file server_file_info.hpp
 * @brief 服务器文件信息
 * @author DaneJoe001
 */

#include <string>
#include <format>
#include <cstdint>

/**
 * @class ServerFileInfo
 * @brief 服务器文件信息
 */
struct ServerFileInfo
{
    /// @brief 文件ID
    int32_t file_id;
    /// @brief 文件名称
    std::string file_name;
    /// @brief 资源路径
    std::string resource_path;
    /// @brief 文件大小
    uint32_t file_size;
    /// @brief MD5码
    std::string md5_code;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
    /**
     * @brief 判断是否相等
     * @param other 其他文件信息
     * @return 是否相等
     */
    bool operator==(const ServerFileInfo& other)const;
};

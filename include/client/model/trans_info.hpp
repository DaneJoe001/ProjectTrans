#pragma once

/**
 * @file trans_info.hpp
 * @brief 传输信息
 * @author DaneJoe001
 */

#include <cstdint>

#include "client/model/client_file_info.hpp"

/**
 * @struct TransInfo
 * @brief 传输信息
 */
struct TransInfo
{
    /// @brief 文件信息
    ClientFileInfo file_info;
    /// @brief 当前数量
    uint32_t current_count;
    /// @brief 总数量
    uint32_t total_count;
};

/**
 * @struct DownloadRequest
 * @brief 下载请求
 */
struct DownloadRequest
{
    /// @brief 文件URL
    std::string file_url;
    /// @brief 用户名
    std::string username;
    /// @brief 密码
    std::string password;
};
#pragma once

/**
 * @file client_file_info.hpp
 * @brief 客户端文件信息
 * @author DaneJoe001
 */

#include <string>
#include <chrono>
#include <format>
#include <cstdint>

#include "client/model/common.hpp"

/**
 * @class ClientFileInfo
 * @brief 客户端文件信息
 */
struct ClientFileInfo
{
    /// @brief 文件ID
    int32_t file_id = -1;
    /// @brief 保存名称
    std::string saved_name;
    /// @brief 源路径
    std::string source_path;
    /// @brief 保存路径
    std::string saved_path;
    /// @brief 文件大小
    uint32_t file_size = 0;
    /// @brief 操作
    Operation operation = Operation::Unknown;
    /// @brief 文件状态
    FileState state = FileState::Unknown;
    /// @brief MD5码
    std::string md5_code;
    /// @brief 创建时间
    std::chrono::time_point<std::chrono::system_clock> create_time;
    /// @brief 完成时间
    std::chrono::time_point<std::chrono::system_clock> finished_time;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

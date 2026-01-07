/**
 * @file client_file_entity.hpp
 * @brief 客户端文件信息
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <string>
#include <cstdint>

/**
 * @struct ClientFileEntity
 * @brief 客户端文件信息
 * @details 描述客户端侧文件元信息（文件ID、文件名、大小与校验信息）。
 */
struct ClientFileEntity
{
  /// @brief 文件ID
  int64_t file_id = -1;
  /// @brief 文件名称
  /// @note 源自服务器，可作为默认文件名
  std::string file_name;
  
  /// @brief 文件大小
  int64_t file_size = 0;
  /// @brief MD5码
  std::string md5_code;
  /**
   * @brief 转换为字符串
   * @return 字符串
   * @details 将文件元信息转换为字符串描述。
   */
  std::string to_string() const;
};

/**
 * @file client_file_info.hpp
 * @brief 客户端文件信息
 * @author DaneJoe001
 */
#pragma once

#include <string>
#include <cstdint>

 /**
  * @class ClientFileInfo
  * @brief 客户端文件信息
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
   */
  std::string to_string() const;
};

#pragma once

/**
 * @file client_file_info.hpp
 * @brief 客户端文件信息
 * @author DaneJoe001
 */

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
  /// @brief 保存名称
  std::string saved_name;
  /// @brief 源路径
  std::string source_path;
  /// @brief 保存路径
  std::string saved_path;
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

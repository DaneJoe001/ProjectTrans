#pragma once

/**
 * @file block_request_info.hpp
 * @brief 请求块信息
 * @author DaneJoe001
 */

#include <chrono>
#include <format>
#include <cstdint>

#include "client/model/common.hpp"

struct BlockParamConfig
{
  /// @brief 块最小尺寸
  uint32_t min_block_size = 512;
};

/**
 * @class BlockRequestInfo
 * @brief 请求块信息
 */
struct BlockRequestInfo
{
  /// @brief 请求块ID(由数据库自动生成)
  int32_t block_id = -1;
  /// @brief 文件ID
  int32_t file_id;
  /// @brief 块偏移
  uint32_t offset;
  /// @brief 块大小
  uint32_t block_size;
  /// @brief 操作
  Operation operation;
  /// @brief 文件状态
  FileState state;
  /// @brief 开始时间
  std::chrono::time_point<std::chrono::steady_clock> start_time;
  /// @brief 结束时间
  std::chrono::time_point<std::chrono::steady_clock> end_time;
  /**
   * @brief 转换为字符串
   * @return 字符串
   */
  std::string to_string() const;
};
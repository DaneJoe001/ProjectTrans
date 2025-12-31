/**
 * @file common_enum.hpp
 * @brief 通用枚举
 * @author DaneJoe001
 * @date 2025-12-28
 */
#pragma once

#include <string>

/**
 * @enum FileState
 * @brief 文件状态
 */
enum class FileState {
  /// @brief 未知
  Unknown,
  /// @brief 等待
  Waiting,
  /// @brief 传输中
  InTransfer,
  /// @brief 完成
  Completed,
  /// @brief 失败
  Failed,
};

/**
 * @brief 将文件状态枚举转换为字符串
 * @param state 文件状态
 * @return 文件状态的字符串表示
 */
std::string to_string(FileState state);

#pragma once

/**
 * @file common.hpp
 * @brief 通用枚举
 * @author DaneJoe001
 */

#include <string>

 /**
  * @enum FileState
  * @brief 文件状态
  */
enum class FileState
{
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
 * @enum Operation
 * @brief 操作
 */
enum class Operation
{
    /// @brief 未知
    Unknown,
    /// @brief 下载
    Download,
    /// @brief 上传
    Upload,
};

/**
 * @brief 将文件状态枚举转换为字符串
 * @param state 文件状态
 * @return 文件状态的字符串表示
 */
std::string to_string(FileState state);

/**
 * @brief 将操作枚举转换为字符串
 * @param operation 操作
 * @return 操作的字符串表示
 */
std::string to_string(Operation operation);
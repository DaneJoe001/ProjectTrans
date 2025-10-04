#pragma once

#include <string>

/**
 * @file common.hpp
 * @brief 通用枚举
 * @author DaneJoe001
 */


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

std::string to_string(FileState state);

std::string to_string(Operation operation);
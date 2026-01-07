/**
  * @file task_entity.hpp
  * @brief 任务实体
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <cstdint>
#include <chrono>
#include <string>

/**
 * @enum TaskState
 * @brief 任务状态
 * @details 描述任务在创建、传输与完成过程中的生命周期状态。
 */
enum class TaskState
{
    /// @brief 完成
    Completed,
    /// @brief 失败
    Failed,
    /// @brief 传输中
    InTransfer,
    /// @brief 等待
    Waiting,
    /// @brief 未知
    Unknown,
};

/**
 * @brief 将任务状态转换为字符串
 * @param state 任务状态
 * @return 状态对应的字符串
 */
std::string to_string(TaskState state);

/**
 * @enum Operation
 * @brief 操作
 * @details 描述任务对应的操作类型（下载/上传）。
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
 * @brief 将操作枚举转换为字符串
 * @param operation 操作
 * @return 操作的字符串表示
 */
std::string to_string(Operation operation);

/**
 * @struct TaskEntity
 * @brief 任务信息实体
 * @details 描述任务元信息（关联文件、保存路径、来源、操作类型、状态与时间戳等）。
 */
struct TaskEntity
{
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
    /// @brief 保存路径
    /// @note full_path
    std::string saved_path;
    /// @brief 源链接
    std::string source_url;
    /// @brief 操作
    Operation operation = Operation::Unknown;
    /// @brief 任务状态
    TaskState state;
    /// @brief 开始时间
    std::chrono::time_point<std::chrono::system_clock> start_time;
    /// @brief 结束时间
    std::chrono::time_point<std::chrono::system_clock> end_time;
    /**
     * @brief 转换为字符串
     * @return 字符串
     * @details 将任务信息实体的各个字段转换为字符串描述。
     */
    std::string to_string() const;
};

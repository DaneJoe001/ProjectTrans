#pragma once

#include <cstdint>
#include <chrono>
 #include <string>

/**
 * @enum TaskState
 * @brief 任务状态
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

std::string to_string(TaskState state);

/**
* @brief 将操作枚举转换为字符串
* @param operation 操作
* @return 操作的字符串表示
*/
std::string to_string(Operation operation);

/**
 * @struct TaskEntity
 * @brief 任务信息实体
 */
struct TaskEntity
{
    /// @brief 任务ID
    int64_t task_id = -1;
    /// @brief 文件ID
    int64_t file_id = -1;
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
     */
    std::string to_string() const;
};
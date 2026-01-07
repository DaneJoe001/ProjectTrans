/**
 * @file task_service.hpp
 * @brief 任务服务
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <atomic>

#include "model/entity/task_entity.hpp"
#include "repository/task_repository.hpp"

 /**
  * @class TaskService
  * @brief 任务服务
  * @details 提供任务信息（TaskEntity）的增删改查接口，并通过 TaskRepository 进行持久化管理。
  */
class TaskService
{
public:
    /**
     * @brief 构造函数
     */
    TaskService();
    /**
     * @brief 析构函数
     */
    ~TaskService();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取所有任务
     * @return 任务列表
     */
    std::vector<TaskEntity> get_all();
    /**
     * @brief 通过任务ID获取任务
     * @param task_id 任务ID
     * @return 任务
     */
    std::optional<TaskEntity> get_by_task_id(int64_t task_id);
    /**
     * @brief 通过文件ID获取任务
     * @param file_id 文件ID
     * @return 任务列表
     */
    std::vector<TaskEntity> get_by_file_id(int64_t file_id);
    /**
     * @brief 添加任务
     * @param task_entity 任务
     * @return 是否成功
     */
    bool add(TaskEntity& task_entity);
    /**
     * @brief 更新任务
     * @param task_entity 任务
     * @return 是否成功
     */
    bool update(const TaskEntity& task_entity);
    /**
     * @brief 删除任务
     * @param task_id 任务ID
     * @return 是否成功
     */
    bool remove(int64_t task_id);
    /**
     * @brief 获取任务总数
     * @return 任务数量
     */
    int64_t get_all_count();
private:
    /// @brief 任务计数器
    std::atomic<int64_t> m_task_counter = -1;
    /// @brief 任务仓库
    TaskRepository m_task_repository;
};

/**
  * @file task_repository.hpp
  * @brief 任务信息仓库
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sql_table_query.hpp>

#include "model/entity/task_entity.hpp"

  /**
    * @class TaskRepository
    * @brief 任务信息仓库
    * @details 负责对 TaskEntity 进行数据库持久化读写操作。
    */
class TaskRepository
{
public:
    /**
     * @brief 构造函数
     */
    TaskRepository();
    /**
     * @brief 析构函数
     */
    ~TaskRepository();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 确保表存在
     * @return 是否成功
     */
    bool ensure_table_exists();
    /**
     * @brief 获取全部任务
     * @return 任务列表
     */
    std::vector<TaskEntity> get_all();
    /**
     * @brief 通过任务ID获取任务
     * @param task_id 任务ID
     * @return 任务（不存在时返回空）
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
     * @param task 任务信息
     * @return 是否成功
     */
    bool add(const TaskEntity& task);
    /**
     * @brief 更新任务
     * @param task 任务信息
     * @return 是否成功
     */
    bool update(const TaskEntity& task);
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
    /**
     * @brief 获取最大任务ID
     * @return 最大任务ID
     */
    int64_t get_max_task_id();
private:
    /**
     * @brief 将查询结果转换为任务信息实体列表
     * @param data 查询结果数据
     * @return 任务信息实体列表
     */
    std::vector<TaskEntity> from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 数据表信息
    std::shared_ptr<DaneJoe::SqlTableItem> m_table;
    /// @brief 数据表查询器
    DaneJoe::SqlTableQuery m_table_query;
};

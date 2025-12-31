#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sql_table_query.hpp>

#include "model/entity/task_entity.hpp"

class TaskRepository
{
public:
    TaskRepository();
    ~TaskRepository();

    void init();
    bool ensure_table_exists();

    std::vector<TaskEntity> get_all();
    std::optional<TaskEntity> get_by_task_id(int64_t task_id);
    std::vector<TaskEntity> get_by_file_id(int64_t file_id);

    bool add(const TaskEntity& task);
    bool update(const TaskEntity& task);
    bool remove(int64_t task_id);

    int64_t get_all_count();
    int64_t get_max_task_id();
private:
    std::vector<TaskEntity> from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data);

private:
    bool m_is_init = false;
    std::shared_ptr<DaneJoe::SqlTableItem> m_table;
    DaneJoe::SqlTableQuery m_table_query;
};

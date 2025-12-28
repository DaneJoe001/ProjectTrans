#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sqlite_stringify.hpp>
#include <danejoe/condition/range_condition.hpp>
#include <danejoe/logger/logger_manager.hpp>

#include "repository/task_repository.hpp"

TaskRepository::TaskRepository() {}

TaskRepository::~TaskRepository() {}

void TaskRepository::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database already initialized");
        return;
    }

    m_table = std::make_shared<DaneJoe::SqlTableItem>();

    auto sql_database = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
    auto sql_query = std::make_shared<DaneJoe::SqlQuery>(sql_database);
    auto sqlite_stringify = std::make_shared<DaneJoe::SqliteStringify>();

    m_table->table_name = "task";
    m_table->is_unique = true;

    DaneJoe::SqlColumnItem task_id_column;
    task_id_column.column_index = 0;
    task_id_column.column_name = "task_id";
    task_id_column.data_type = DaneJoe::DataType::Int64;
    task_id_column.is_primary_key = true;
    task_id_column.is_not_null = true;
    task_id_column.is_unique = true;
    m_table->add_column(task_id_column);

    DaneJoe::SqlColumnItem file_id_column;
    file_id_column.column_index = 1;
    file_id_column.column_name = "file_id";
    file_id_column.data_type = DaneJoe::DataType::Int64;
    file_id_column.is_not_null = true;
    m_table->add_column(file_id_column);

    DaneJoe::SqlColumnItem operation_column;
    operation_column.column_index = 2;
    operation_column.column_name = "operation";
    operation_column.data_type = DaneJoe::DataType::Int64;
    operation_column.is_not_null = true;
    m_table->add_column(operation_column);

    DaneJoe::SqlColumnItem state_column;
    state_column.column_index = 3;
    state_column.column_name = "state";
    state_column.data_type = DaneJoe::DataType::Int64;
    state_column.is_not_null = true;
    m_table->add_column(state_column);

    DaneJoe::SqlColumnItem start_time_column;
    start_time_column.column_index = 4;
    start_time_column.column_name = "start_time";
    start_time_column.data_type = DaneJoe::DataType::Int64;
    start_time_column.is_not_null = true;
    m_table->add_column(start_time_column);

    DaneJoe::SqlColumnItem end_time_column;
    end_time_column.column_index = 5;
    end_time_column.column_name = "end_time";
    end_time_column.data_type = DaneJoe::DataType::Int64;
    end_time_column.is_not_null = true;
    m_table->add_column(end_time_column);

    m_table_query.set_stringify(sqlite_stringify);
    m_table_query.set_table_info(m_table);
    m_table_query.set_query(sql_query);

    m_is_init = true;
    m_is_init = ensure_table_exists();
}

bool TaskRepository::ensure_table_exists()
{
    if (!m_is_init || !m_table)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return false;
    }
    return m_table_query.create();
}

std::vector<TaskEntity> TaskRepository::get_all()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return {};
    }
    auto data = m_table_query.select(m_table->column_items, {});
    return from_query_data(data);
}

std::optional<TaskEntity> TaskRepository::get_by_task_id(int64_t task_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return std::nullopt;
    }

    DaneJoe::SqlConditionItem condition;
    auto task_id_column_opt = m_table->get_column_info("task_id");
    if (!task_id_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Table column not found");
        return std::nullopt;
    }

    condition.column_info = *task_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(task_id);

    auto data = m_table_query.select(m_table->column_items, { condition });
    auto result = from_query_data(data);
    if (result.empty())
    {
        return std::nullopt;
    }
    return result[0];
}

std::vector<TaskEntity> TaskRepository::get_by_file_id(int64_t file_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return {};
    }

    DaneJoe::SqlConditionItem condition;
    auto file_id_column_opt = m_table->get_column_info("file_id");
    if (!file_id_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Table column not found");
        return {};
    }

    condition.column_info = *file_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(file_id);

    auto data = m_table_query.select(m_table->column_items, { condition });
    return from_query_data(data);
}

bool TaskRepository::add(const TaskEntity& task)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return false;
    }

    auto task_id_column_opt = m_table->get_column_info("task_id");
    auto file_id_column_opt = m_table->get_column_info("file_id");
    auto operation_column_opt = m_table->get_column_info("operation");
    auto state_column_opt = m_table->get_column_info("state");
    auto start_time_column_opt = m_table->get_column_info("start_time");
    auto end_time_column_opt = m_table->get_column_info("end_time");
    if (!task_id_column_opt || !file_id_column_opt || !operation_column_opt || !state_column_opt || !start_time_column_opt || !end_time_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Table column not found");
        return false;
    }

    auto task_id_cell = task_id_column_opt->get_default_cell();
    task_id_cell.data = task.task_id;

    auto file_id_cell = file_id_column_opt->get_default_cell();
    file_id_cell.data = task.file_id;

    auto operation_cell = operation_column_opt->get_default_cell();
    operation_cell.data = static_cast<int64_t>(task.operation);

    auto state_cell = state_column_opt->get_default_cell();
    state_cell.data = static_cast<int64_t>(task.state);

    auto start_time_cell = start_time_column_opt->get_default_cell();
    start_time_cell.data = DaneJoe::to_time_ms(task.start_time);

    auto end_time_cell = end_time_column_opt->get_default_cell();
    end_time_cell.data = DaneJoe::to_time_ms(task.end_time);

    return m_table_query.insert({ task_id_cell, file_id_cell, operation_cell, state_cell, start_time_cell, end_time_cell });
}

bool TaskRepository::update(const TaskEntity& task)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return false;
    }

    auto task_id_column_opt = m_table->get_column_info("task_id");
    auto file_id_column_opt = m_table->get_column_info("file_id");
    auto operation_column_opt = m_table->get_column_info("operation");
    auto state_column_opt = m_table->get_column_info("state");
    auto start_time_column_opt = m_table->get_column_info("start_time");
    auto end_time_column_opt = m_table->get_column_info("end_time");
    if (!task_id_column_opt || !file_id_column_opt || !operation_column_opt || !state_column_opt || !start_time_column_opt || !end_time_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Table column not found");
        return false;
    }

    auto file_id_cell = file_id_column_opt->get_default_cell();
    file_id_cell.data = task.file_id;

    auto operation_cell = operation_column_opt->get_default_cell();
    operation_cell.data = static_cast<int64_t>(task.operation);

    auto state_cell = state_column_opt->get_default_cell();
    state_cell.data = static_cast<int64_t>(task.state);

    auto start_time_cell = start_time_column_opt->get_default_cell();
    start_time_cell.data = DaneJoe::to_time_ms(task.start_time);

    auto end_time_cell = end_time_column_opt->get_default_cell();
    end_time_cell.data = DaneJoe::to_time_ms(task.end_time);

    DaneJoe::SqlConditionItem condition;
    condition.column_info = *task_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(task.task_id);

    return m_table_query.update({ file_id_cell, operation_cell, state_cell, start_time_cell, end_time_cell }, { condition });
}

bool TaskRepository::remove(int64_t task_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Database not initialized");
        return false;
    }

    DaneJoe::SqlConditionItem condition;
    auto task_id_column_opt = m_table->get_column_info("task_id");
    if (!task_id_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "TaskRepository", "Table column not found");
        return false;
    }

    condition.column_info = *task_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(task_id);

    return m_table_query.remove({ condition });
}

std::vector<TaskEntity> TaskRepository::from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data)
{
    std::vector<TaskEntity> result;
    for (const auto& row : data)
    {
        TaskEntity task;
        for (const auto& cell : row)
        {
            if (cell.column_name == "task_id")
            {
                task.task_id = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "file_id")
            {
                task.file_id = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "operation")
            {
                task.operation = static_cast<Operation>(std::get<int64_t>(cell.data));
            }
            else if (cell.column_name == "state")
            {
                task.state = static_cast<TaskState>(std::get<int64_t>(cell.data));
            }
            else if (cell.column_name == "start_time")
            {
                task.start_time = DaneJoe::from_time_ms<std::chrono::system_clock::time_point>(std::get<int64_t>(cell.data));
            }
            else if (cell.column_name == "end_time")
            {
                task.end_time = DaneJoe::from_time_ms<std::chrono::system_clock::time_point>(std::get<int64_t>(cell.data));
            }
        }
        result.push_back(task);
    }
    return result;
}

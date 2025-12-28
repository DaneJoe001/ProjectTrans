#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sqlite_stringify.hpp>
#include <danejoe/database/sqlite_driver.hpp>
#include <danejoe/condition/range_condition.hpp>

#include "repository/block_repository.hpp"
#include "danejoe/database/sql_condition.hpp"

BlockRepository::BlockRepository() {}

BlockRepository::~BlockRepository() {}

void BlockRepository::init()
{
    // 当内部数据库已经初始化时直接返回
    if (m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database already initialized");
        return;
    }
    m_table =
        std::make_shared<DaneJoe::SqlTableItem>();
    auto sql_database = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
    auto sql_query =
        std::make_shared<DaneJoe::SqlQuery>(sql_database);
    auto sqlite_stringify =
        std::make_shared<DaneJoe::SqliteStringify>();
    m_table->table_name = "block";
    m_table->is_unique = true;
    DaneJoe::SqlColumnItem block_id_column;
    block_id_column.column_index = 0;
    block_id_column.column_name = "block_id";
    block_id_column.data_type = DaneJoe::DataType::Int64;
    block_id_column.is_primary_key = true;
    block_id_column.is_not_null = true;
    block_id_column.is_unique = true;
    m_table->add_column(block_id_column);
    DaneJoe::SqlColumnItem task_id_column;
    task_id_column.column_index = 1;
    task_id_column.column_name = "task_id";
    task_id_column.data_type = DaneJoe::DataType::Int64;
    task_id_column.is_not_null = true;
    m_table->add_column(task_id_column);
    DaneJoe::SqlColumnItem offset_column;
    offset_column.column_index = 2;
    offset_column.column_name = "offset";
    offset_column.data_type = DaneJoe::DataType::Int64;
    offset_column.is_not_null = true;
    m_table->add_column(offset_column);
    DaneJoe::SqlColumnItem block_size_column;
    block_size_column.column_index = 3;
    block_size_column.column_name = "block_size";
    block_size_column.data_type = DaneJoe::DataType::Int64;
    block_size_column.is_not_null = true;
    m_table->add_column(block_size_column);
    DaneJoe::SqlColumnItem state_column;
    state_column.column_index = 4;
    state_column.column_name = "state";
    state_column.data_type = DaneJoe::DataType::Int64;
    state_column.is_not_null = true;
    m_table->add_column(state_column);
    DaneJoe::SqlColumnItem start_time_column;
    start_time_column.column_index = 5;
    start_time_column.column_name = "start_time";
    start_time_column.data_type = DaneJoe::DataType::Int64;
    start_time_column.is_not_null = true;
    m_table->add_column(start_time_column);
    DaneJoe::SqlColumnItem end_time_column;
    end_time_column.column_index = 6;
    end_time_column.column_name = "end_time";
    end_time_column.data_type = DaneJoe::DataType::Int64;
    end_time_column.is_not_null = true;
    m_table->add_column(end_time_column);
    m_table_query.set_stringify(sqlite_stringify);
    m_table_query.set_table_info(m_table);
    m_table_query.set_query(sql_query);
    // 标识仓库已完成基本配置；确保 ensure_table_exists 不会因 m_is_init=false 而短路
    m_is_init = true;
    m_is_init = ensure_table_exists();
}

bool BlockRepository::ensure_table_exists()
{
    // 判断仓库是否完成基本配置
    if (!m_is_init || !m_table)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return false;
    }
    return m_table_query.create();
}



std::vector<BlockEntity> BlockRepository::get_all()
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return std::vector<BlockEntity>();
    }
    auto data =
        m_table_query.select(m_table->column_items, {});
    return from_query_data(data);
}

std::vector<BlockEntity> BlockRepository::get_by_task_id(int64_t task_id)
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return std::vector<BlockEntity>();
    }

    DaneJoe::SqlConditionItem condition;
    auto task_id_column_opt =
        m_table->get_column_info("task_id");
    if (!task_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return std::vector<BlockEntity>();
    }
    condition.column_info = task_id_column_opt.value();
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    auto range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(task_id);
    condition.condition = range_condition;
    auto data =
        m_table_query.select(m_table->column_items, { condition });
    return from_query_data(data);
}

int64_t BlockRepository::get_count_by_task_id_and_block_state(int64_t task_id, BlockState state)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return 0;
    }
    // 构建任务ID条件
    DaneJoe::SqlConditionItem task_id_condition;
    auto task_id_column_opt = m_table->get_column_info("task_id");
    if (!task_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return 0;
    }
    task_id_condition.column_info = task_id_column_opt.value();
    task_id_condition.is_set = true;
    task_id_condition.is_desc_order = std::nullopt;
    auto range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(task_id);
    task_id_condition.condition = range_condition;
    // 构建状态条件
    DaneJoe::SqlConditionItem state_condition;
    auto state_column_opt =
        m_table->get_column_info("state");
    if (!state_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return 0;
    }
    state_condition.column_info = state_column_opt.value();
    state_condition.is_set = true;
    state_condition.is_desc_order = std::nullopt;
    auto state_range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(static_cast<int64_t>(state));
    state_condition.condition = state_range_condition;
    return m_table_query.count({ task_id_condition, state_condition });
}

std::vector<BlockEntity> BlockRepository::get_by_task_id_and_block_state(int64_t task_id, BlockState state)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return std::vector<BlockEntity>();
    }
    DaneJoe::SqlConditionItem task_id_condition;
    auto task_id_column_opt =
        m_table->get_column_info("task_id");
    if (!task_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return std::vector<BlockEntity>();
    }
    task_id_condition.column_info = task_id_column_opt.value();
    task_id_condition.is_set = true;
    task_id_condition.is_desc_order = std::nullopt;
    auto range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(task_id);
    task_id_condition.condition = range_condition;
    // 构建状态条件
    DaneJoe::SqlConditionItem state_condition;
    auto state_column_opt =
        m_table->get_column_info("state");
    if (!state_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return std::vector<BlockEntity>();
    }
    state_condition.column_info = state_column_opt.value();
    state_condition.is_set = true;
    state_condition.is_desc_order = std::nullopt;
    auto state_range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(static_cast<int64_t>(state));
    state_condition.condition = state_range_condition;
    auto data =
        m_table_query.select(m_table->column_items, { task_id_condition, state_condition });
    return from_query_data(data);
}

bool BlockRepository::add(const BlockEntity& block)
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return false;
    }
    auto task_id_column_opt = m_table->get_column_info("task_id");
    if (!task_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto task_id_cell =
        task_id_column_opt.value().get_default_cell();
    task_id_cell.data = block.task_id;
    auto offset_column_opt =
        m_table->get_column_info("offset");
    if (!offset_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto offset_cell =
        offset_column_opt.value().get_default_cell();
    offset_cell.data = block.offset;
    auto block_size_column_opt =
        m_table->get_column_info("block_size");
    if (!block_size_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto block_size_cell =
        block_size_column_opt.value().get_default_cell();
    block_size_cell.data = block.block_size;
    auto state_column_opt =
        m_table->get_column_info("state");
    if (!state_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto state_cell =
        state_column_opt.value().get_default_cell();
    state_cell.data = static_cast<int64_t>(block.state);
    auto start_time_column_opt =
        m_table->get_column_info("start_time");
    if (!start_time_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto start_time_cell =
        start_time_column_opt.value().get_default_cell();
    start_time_cell.data = DaneJoe::to_time_ms(block.start_time);
    auto end_time_column_opt =
        m_table->get_column_info("end_time");
    if (!end_time_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto end_time_cell =
        end_time_column_opt.value().get_default_cell();
    end_time_cell.data = DaneJoe::to_time_ms(block.end_time);
    return m_table_query.insert({ task_id_cell, offset_cell, block_size_cell, state_cell, start_time_cell, end_time_cell });
}
std::optional<BlockEntity> BlockRepository::get_by_block_id(int64_t block_id)
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return std::nullopt;
    }
    DaneJoe::SqlConditionItem condition;
    auto block_id_column_opt =
        m_table->get_column_info("block_id");
    if (!block_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return std::nullopt;
    }
    condition.column_info = block_id_column_opt.value();
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    auto range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(block_id);
    condition.condition = range_condition;
    auto data =
        m_table_query.select(m_table->column_items, { condition });
    auto result = from_query_data(data);
    if (result.empty())
    {
        return std::nullopt;
    }
    return result[0];
}

bool BlockRepository::update(const BlockEntity& block)
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return false;
    }
    auto task_id_column_opt = m_table->get_column_info("task_id");
    if (!task_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto task_id_cell = task_id_column_opt.value().get_default_cell();
    task_id_cell.data = block.task_id;
    auto offset_column_opt = m_table->get_column_info("offset");
    if (!offset_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto offset_cell = offset_column_opt.value().get_default_cell();
    offset_cell.data = block.offset;
    auto block_size_column_opt = m_table->get_column_info("block_size");
    if (!block_size_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto block_size_cell = block_size_column_opt.value().get_default_cell();
    block_size_cell.data = block.block_size;
    auto state_column_opt = m_table->get_column_info("state");
    if (!state_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto state_cell = state_column_opt.value().get_default_cell();
    state_cell.data = static_cast<int64_t>(block.state);
    auto start_time_column_opt = m_table->get_column_info("start_time");
    if (!start_time_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto start_time_cell = start_time_column_opt.value().get_default_cell();
    start_time_cell.data = DaneJoe::to_time_ms(block.start_time);

    auto end_time_column_opt = m_table->get_column_info("end_time");
    if (!end_time_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    auto end_time_cell = end_time_column_opt.value().get_default_cell();
    end_time_cell.data = DaneJoe::to_time_ms(block.end_time);
    DaneJoe::SqlConditionItem condition;
    auto block_id_column_opt =
        m_table->get_column_info("block_id");
    if (!block_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    condition.column_info = block_id_column_opt.value();
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    auto range_condition =
        std::make_shared<DaneJoe::RangeCondition<int64_t>>(block.block_id);
    condition.condition = range_condition;
    return m_table_query.update({ task_id_cell, offset_cell, block_size_cell, state_cell, start_time_cell, end_time_cell }, { condition });
}
bool BlockRepository::remove(int64_t block_id)
{
    // 判断数据库是否初始化
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Database not initialized");
        return false;
    }
    // 执行删除记录并返回是否成功
    DaneJoe::SqlConditionItem condition;
    auto block_id_column_opt = m_table->get_column_info("block_id");
    if (!block_id_column_opt.has_value())
    {
        DANEJOE_LOG_TRACE("default", "BlockRepository", "Table column not found");
        return false;
    }
    condition.column_info = block_id_column_opt.value();
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    auto range_condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(block_id);
    condition.condition = range_condition;
    return m_table_query.remove({ condition });
}

std::vector<BlockEntity>
BlockRepository::from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data)
{
    std::vector<BlockEntity> result;
    for (const auto& row : data)
    {
        BlockEntity block;
        for (const auto& cell : row)
        {
            if (cell.column_name == "block_id")
            {
                block.block_id = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "task_id")
            {
                block.task_id = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "offset")
            {
                block.offset = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "block_size")
            {
                block.block_size = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "state")
            {
                block.state = static_cast<BlockState>(std::get<int64_t>(cell.data));
            }
            else if (cell.column_name == "start_time")
            {
                block.start_time = DaneJoe::from_time_ms<std::chrono::system_clock::time_point>(std::get<int64_t>(cell.data));
            }
            else if (cell.column_name == "end_time")
            {
                block.end_time = DaneJoe::from_time_ms<std::chrono::system_clock::time_point>(std::get<int64_t>(cell.data));
            }
        }
        result.push_back(block);
    }
    return result;
}
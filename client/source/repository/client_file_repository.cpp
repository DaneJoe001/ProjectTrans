#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sqlite_stringify.hpp>
#include <danejoe/condition/range_condition.hpp>
#include <danejoe/logger/logger_manager.hpp>

#include "repository/client_file_repository.hpp"

ClientFileRepository::ClientFileRepository() {}

ClientFileRepository::~ClientFileRepository() {}

void ClientFileRepository::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database already initialized");
        return;
    }

    m_table = std::make_shared<DaneJoe::SqlTableItem>();

    auto sql_database = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
    auto sql_query = std::make_shared<DaneJoe::SqlQuery>(sql_database);
    auto sqlite_stringify = std::make_shared<DaneJoe::SqliteStringify>();

    m_table->table_name = "file_info";
    m_table->is_unique = true;

    DaneJoe::SqlColumnItem file_id_column;
    file_id_column.column_index = 0;
    file_id_column.column_name = "file_id";
    file_id_column.data_type = DaneJoe::DataType::Int64;
    file_id_column.is_primary_key = true;
    file_id_column.is_not_null = true;
    file_id_column.is_unique = true;
    m_table->add_column(file_id_column);

    DaneJoe::SqlColumnItem file_name_column;
    file_name_column.column_index = 1;
    file_name_column.column_name = "file_name";
    file_name_column.data_type = DaneJoe::DataType::String;
    file_name_column.is_not_null = true;
    m_table->add_column(file_name_column);

    DaneJoe::SqlColumnItem file_size_column;
    file_size_column.column_index = 2;
    file_size_column.column_name = "file_size";
    file_size_column.data_type = DaneJoe::DataType::Int64;
    file_size_column.is_not_null = true;
    m_table->add_column(file_size_column);

    DaneJoe::SqlColumnItem md5_code_column;
    md5_code_column.column_index = 3;
    md5_code_column.column_name = "md5_code";
    md5_code_column.data_type = DaneJoe::DataType::String;
    md5_code_column.is_not_null = true;
    m_table->add_column(md5_code_column);

    m_table_query.set_stringify(sqlite_stringify);
    m_table_query.set_table_info(m_table);
    m_table_query.set_query(sql_query);

    m_is_init = true;
    m_is_init = ensure_table_exists();
}

bool ClientFileRepository::ensure_table_exists()
{
    if (!m_is_init || !m_table)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return false;
    }
    return m_table_query.create();
}

std::vector<ClientFileEntity> ClientFileRepository::get_all()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return {};
    }
    auto data = m_table_query.select(m_table->column_items, {});
    return from_query_data(data);
}

bool ClientFileRepository::add(const ClientFileEntity& file_info)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return false;
    }

    auto file_id_column_opt = m_table->get_column_info("file_id");
    auto file_name_column_opt = m_table->get_column_info("file_name");
    auto file_size_column_opt = m_table->get_column_info("file_size");
    auto md5_code_column_opt = m_table->get_column_info("md5_code");
    if (!file_id_column_opt || !file_name_column_opt || !file_size_column_opt || !md5_code_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Table column not found");
        return false;
    }

    auto file_id_cell = file_id_column_opt->get_default_cell();
    file_id_cell.data = file_info.file_id;

    auto file_name_cell = file_name_column_opt->get_default_cell();
    file_name_cell.data = file_info.file_name;

    auto file_size_cell = file_size_column_opt->get_default_cell();
    file_size_cell.data = file_info.file_size;

    auto md5_code_cell = md5_code_column_opt->get_default_cell();
    md5_code_cell.data = file_info.md5_code;

    return m_table_query.insert({ file_id_cell, file_name_cell, file_size_cell, md5_code_cell });
}

std::optional<ClientFileEntity> ClientFileRepository::get_by_id(int64_t file_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return std::nullopt;
    }

    DaneJoe::SqlConditionItem condition;
    auto file_id_column_opt = m_table->get_column_info("file_id");
    if (!file_id_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Table column not found");
        return std::nullopt;
    }

    condition.column_info = *file_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(file_id);

    auto data = m_table_query.select(m_table->column_items, { condition });
    auto result = from_query_data(data);
    if (result.empty())
    {
        return std::nullopt;
    }
    return result[0];
}

std::optional<ClientFileEntity> ClientFileRepository::get_by_md5(const std::string& md5_code)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return std::nullopt;
    }

    auto database = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
    if (!database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not found");
        return std::nullopt;
    }

    DaneJoe::SqlQuery query(database);
    query.prepare("SELECT file_id, file_name, file_size, md5_code FROM file_info WHERE md5_code = ?;");
    query.reset();
    query.bind(1, md5_code);
    auto data = query.execute_query();

    auto result = from_query_data(data);
    if (result.empty())
    {
        return std::nullopt;
    }
    return result[0];
}

bool ClientFileRepository::update(const ClientFileEntity& file_info)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return false;
    }

    auto file_id_column_opt = m_table->get_column_info("file_id");
    auto file_name_column_opt = m_table->get_column_info("file_name");
    auto file_size_column_opt = m_table->get_column_info("file_size");
    auto md5_code_column_opt = m_table->get_column_info("md5_code");
    if (!file_id_column_opt || !file_name_column_opt || !file_size_column_opt || !md5_code_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Table column not found");
        return false;
    }

    auto file_name_cell = file_name_column_opt->get_default_cell();
    file_name_cell.data = file_info.file_name;

    auto file_size_cell = file_size_column_opt->get_default_cell();
    file_size_cell.data = file_info.file_size;

    auto md5_code_cell = md5_code_column_opt->get_default_cell();
    md5_code_cell.data = file_info.md5_code;

    DaneJoe::SqlConditionItem condition;
    condition.column_info = *file_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(file_info.file_id);

    return m_table_query.update({ file_name_cell, file_size_cell, md5_code_cell }, { condition });
}

bool ClientFileRepository::remove(int64_t file_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return false;
    }

    DaneJoe::SqlConditionItem condition;
    auto file_id_column_opt = m_table->get_column_info("file_id");
    if (!file_id_column_opt)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Table column not found");
        return false;
    }

    condition.column_info = *file_id_column_opt;
    condition.is_set = true;
    condition.is_desc_order = std::nullopt;
    condition.condition = std::make_shared<DaneJoe::RangeCondition<int64_t>>(file_id);

    return m_table_query.remove({ condition });
}

int64_t ClientFileRepository::get_all_count()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileRepository", "Database not initialized");
        return -1;
    }
    return m_table_query.count({});
}

std::vector<ClientFileEntity> ClientFileRepository::from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data)
{
    std::vector<ClientFileEntity> result;
    for (const auto& row : data)
    {
        ClientFileEntity info;
        for (const auto& cell : row)
        {
            if (cell.column_name == "file_id")
            {
                info.file_id = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "file_name")
            {
                info.file_name = std::get<std::string>(cell.data);
            }
            else if (cell.column_name == "file_size")
            {
                info.file_size = std::get<int64_t>(cell.data);
            }
            else if (cell.column_name == "md5_code")
            {
                info.md5_code = std::get<std::string>(cell.data);
            }
        }
        result.push_back(info);
    }
    return result;
}

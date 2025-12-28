#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sql_table_query.hpp>

#include "model/entity/client_file_entity.hpp"

class ClientFileRepository
{
public:
    ClientFileRepository();
    ~ClientFileRepository();

    void init();
    bool ensure_table_exists();

    std::vector<ClientFileEntity> get_all();

    bool add(const ClientFileEntity& file_info);
    std::optional<ClientFileEntity> get_by_id(int64_t file_id);
    std::optional<ClientFileEntity> get_by_md5(const std::string& md5_code);
    bool update(const ClientFileEntity& file_info);
    bool remove(int64_t file_id);

private:
    std::vector<ClientFileEntity> from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data);

private:
    bool m_is_init = false;
    std::shared_ptr<DaneJoe::SqlTableItem> m_table;
    DaneJoe::SqlTableQuery m_table_query;
};

#pragma once

#include <optional>
#include <vector>
#include <cstdint>

#include "client/model/client_file_info.hpp"
#include "common/database/i_database.hpp"

class ClientFileInfoRepository {
public:
    ClientFileInfoRepository();
    ~ClientFileInfoRepository();
    bool ensure_table_exists();
    void init();
    std::vector<ClientFileInfo> get_all();
    bool add(const ClientFileInfo& file_info);
    std::optional<ClientFileInfo> get_by_id(int32_t file_id);
    std::optional<ClientFileInfo> get_by_md5(const std::string& md5_code);
    bool update(const ClientFileInfo& file_info);
    bool remove(int32_t file_id);
private:
    std::shared_ptr<IDatabase> m_database;
};
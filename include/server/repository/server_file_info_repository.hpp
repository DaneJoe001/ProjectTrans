#pragma once

#include <optional>
#include <vector>

#include "server/model/server_file_info.hpp"
#include "common/database/i_database.hpp"

class ServerFileInfoRepository {
public:
    ServerFileInfoRepository();
    ~ServerFileInfoRepository();
    bool ensure_table_exists();
    void init();
    int count();
    std::vector<ServerFileInfo> get_all();
    bool add(const ServerFileInfo& file_info);
    std::optional<ServerFileInfo> get_by_id(int file_id);
    std::optional<ServerFileInfo> get_by_md5(const std::string& md5_code);
    bool update(const ServerFileInfo& file_info);
    bool remove(int file_id);
private:
    std::shared_ptr<IDatabase> m_database;
};
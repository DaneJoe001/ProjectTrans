#pragma once

#include <optional>
#include <vector>

#include "server/model/file_info.hpp"
#include "common/database/i_database.hpp"

class FileInfoRepository {
public:
    FileInfoRepository();
    ~FileInfoRepository();
    bool ensure_table_exists();
    void init();
    std::vector<FileInfo> get_all();
    bool add(const FileInfo& file_info);
    std::optional<FileInfo> get_by_id(int file_id);
    bool update(const FileInfo& file_info);
    bool remove(int file_id);
private:
    std::shared_ptr<IDatabase> m_database;
};
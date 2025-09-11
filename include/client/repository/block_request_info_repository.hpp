#pragma once

#include <optional>

#include "client/model/block_request_info.hpp"
#include "common/database/i_database.hpp"

class BlockRequestInfoRepository
{
public:
    BlockRequestInfoRepository();
    ~BlockRequestInfoRepository();
    bool ensure_table_exists();
    void init();
    std::vector<BlockRequestInfo> get_all();
    bool add(const BlockRequestInfo& block_info);
    std::optional<BlockRequestInfo> get_by_id(int block_id);
    bool update(const BlockRequestInfo& block_info);
    bool remove(int block_id);
private:
    std::shared_ptr<IDatabase> m_database;
};
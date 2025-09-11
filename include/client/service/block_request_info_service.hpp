#pragma once

#include "client/repository/block_request_info_repository.hpp"

class BlockRequestInfoService
{
public:
    std::vector<BlockRequestInfo> get_all();
    bool add(const BlockRequestInfo& block_info);
    std::optional<BlockRequestInfo> get_by_id(int block_id);
    bool update(const BlockRequestInfo& block_info);
    bool remove(int block_id);
private:
    BlockRequestInfoRepository block_request_info_repository;
};
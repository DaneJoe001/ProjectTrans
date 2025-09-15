#pragma once

#include "client/repository/block_request_info_repository.hpp"

class BlockRequestInfoService
{
public:
    std::vector<BlockRequestInfo> get_all();
    void init();
    bool add(const BlockRequestInfo& block_info);
    std::optional<BlockRequestInfo> get_by_id(int32_t block_id);
    bool update(const BlockRequestInfo& block_info);
    bool remove(int32_t block_id);
private:
    BlockRequestInfoRepository block_request_info_repository;
};
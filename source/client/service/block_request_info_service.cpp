#include "client/service/block_request_info_service.hpp"

std::vector<BlockRequestInfo> BlockRequestInfoService::get_all()
{
    return block_request_info_repository.get_all();
}

void BlockRequestInfoService::init()
{
    block_request_info_repository.init();
}
bool BlockRequestInfoService::add(const BlockRequestInfo& block_info)
{
    return block_request_info_repository.add(block_info);
}
std::optional<BlockRequestInfo> BlockRequestInfoService::get_by_id(int32_t block_id)
{
    return block_request_info_repository.get_by_id(block_id);
}
bool BlockRequestInfoService::update(const BlockRequestInfo& block_info)
{
    return block_request_info_repository.update(block_info);
}

bool BlockRequestInfoService::remove(int32_t block_id)
{
    return block_request_info_repository.remove(block_id);
}
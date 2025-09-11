#include "client/service/block_request_info_service.hpp"

std::vector<BlockRequestInfo> BlockRequestInfoService::get_all()
{
    return block_request_info_repository.get_all();
}
bool BlockRequestInfoService::add(const BlockRequestInfo& file_info)
{
    return block_request_info_repository.add(file_info);
}
std::optional<BlockRequestInfo> BlockRequestInfoService::get_by_id(int file_id)
{
    return block_request_info_repository.get_by_id(file_id);
}
bool BlockRequestInfoService::update(const BlockRequestInfo& file_info)
{
    return block_request_info_repository.update(file_info);
}

bool BlockRequestInfoService::remove(int file_id)
{
    return block_request_info_repository.remove(file_id);
}
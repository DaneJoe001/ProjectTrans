#include "client/service/block_request_info_service.hpp"
#include "log/manage_logger.hpp"

std::vector<BlockRequestInfo> BlockRequestInfoService::get_all()
{
    // 当仓库没有初始化时返回错误
    if (!block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::vector<BlockRequestInfo>();
    }
    // 返回仓库中所有数据
    return block_request_info_repository.get_all();
}

void BlockRequestInfoService::init()
{
    // 当仓库已经初始化时直接返回
    if (block_request_info_repository.is_init())
    {
        return;
    }
    // 初始化仓库
    block_request_info_repository.init();
}
bool BlockRequestInfoService::add(const BlockRequestInfo& block_info)
{
    // 当仓库没有初始化时返回错误
    if (!block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return block_request_info_repository.add(block_info);
}
std::optional<BlockRequestInfo> BlockRequestInfoService::get_by_id(int32_t block_id)
{
    // 当仓库没有初始化时返回错误
    if (!block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::nullopt;
    }
    return block_request_info_repository.get_by_id(block_id);
}
bool BlockRequestInfoService::update(const BlockRequestInfo& block_info)
{
    // 当仓库没有初始化时返回错误
    if (!block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return block_request_info_repository.update(block_info);
}

bool BlockRequestInfoService::remove(int32_t block_id)
{
    // 当仓库没有初始化时返回错误
    if (!block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return block_request_info_repository.remove(block_id);
}
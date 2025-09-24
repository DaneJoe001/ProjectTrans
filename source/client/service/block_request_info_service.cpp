#include "client/service/block_request_info_service.hpp"
#include "log/manage_logger.hpp"

std::vector<BlockRequestInfo> BlockRequestInfoService::get_all()
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::vector<BlockRequestInfo>();
    }
    // 返回仓库中所有数据
    return m_block_request_info_repository.get_all();
}

void BlockRequestInfoService::init()
{
    // 当仓库已经初始化时直接返回
    if (m_block_request_info_repository.is_init())
    {
        return;
    }
    // 初始化仓库
    m_block_request_info_repository.init();
}
bool BlockRequestInfoService::add(const BlockRequestInfo& block_info)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return m_block_request_info_repository.add(block_info);
}
std::optional<BlockRequestInfo> BlockRequestInfoService::get_by_id(int32_t block_id)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::nullopt;
    }
    return m_block_request_info_repository.get_by_id(block_id);
}

std::vector<BlockRequestInfo> BlockRequestInfoService::get_by_file_id(int32_t file_id)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::vector<BlockRequestInfo>();
    }
    return m_block_request_info_repository.get_by_file_id(file_id);
}

std::vector<BlockRequestInfo> BlockRequestInfoService::get_by_file_id_and_state(int32_t file_id, FileState state)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return std::vector<BlockRequestInfo>();
    }
    return m_block_request_info_repository.get_by_file_id_and_state(file_id, state);
}

int64_t BlockRequestInfoService::get_count_by_file_id_and_state(int32_t file_id, FileState state)
{
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return -1;
    }
    return m_block_request_info_repository.get_count_by_file_id_and_state(file_id, state);
}

bool BlockRequestInfoService::update(const BlockRequestInfo& block_info)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return m_block_request_info_repository.update(block_info);
}

bool BlockRequestInfoService::remove(int32_t block_id)
{
    // 当仓库没有初始化时返回错误
    if (!m_block_request_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "BlockRequestInfoService", "Failed to add block info, repository is not init");
        return false;
    }
    return m_block_request_info_repository.remove(block_id);
}
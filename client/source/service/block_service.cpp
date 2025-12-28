#include <danejoe/logger/logger_manager.hpp>

#include "service/block_service.hpp"

BlockService& BlockService::get_instance()
{
    static BlockService instance;
    return instance;
}

std::vector<BlockEntity> BlockService::get_all()
{
    // 返回仓库中所有数据
    return m_block_repository.get_all();
}

void BlockService::init()
{
    // 初始化仓库
    m_block_repository.init();
}
bool BlockService::add(const BlockEntity& block_info)
{
    return m_block_repository.add(block_info);
}
std::optional<BlockEntity> BlockService::get_by_id(int64_t block_id)
{
    return m_block_repository.get_by_block_id(block_id);
}

std::vector<BlockEntity> BlockService::get_by_task_id(int64_t task_id)
{
    return m_block_repository.get_by_task_id(task_id);
}

std::vector<BlockEntity> BlockService::get_by_task_id_and_block_state(int64_t task_id, BlockState state)
{
    return m_block_repository.get_by_task_id_and_block_state(task_id, state);
}

int64_t BlockService::get_count_by_task_id_and_block_state(int64_t task_id, BlockState state)
{
    return m_block_repository.get_count_by_task_id_and_block_state(task_id, state);
}

bool BlockService::update(const BlockEntity& block_info)
{
    return m_block_repository.update(block_info);
}

bool BlockService::remove(int64_t block_id)
{
    return m_block_repository.remove(block_id);
}
#include <danejoe/logger/logger_manager.hpp>

#include "service/block_service.hpp"

BlockService::BlockService() {}
BlockService::~BlockService() {}

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
bool BlockService::add(const ClientFileEntity& file_entity, int64_t task_id, const BlockParam& param)
{
    int64_t offset = 0;
    bool is_succeed = false;
    while (offset < file_entity.file_size)
    {
        BlockEntity block_entity;
        block_entity.file_id = file_entity.file_id;
        block_entity.offset = offset;
        block_entity.block_size =
            file_entity.file_size - offset >= param.block_bytes ?
            param.block_bytes :
            file_entity.file_size - offset;
        block_entity.task_id = task_id;
        block_entity.state = BlockState::Waiting;
        offset += block_entity.block_size;
        is_succeed = add(block_entity);
        if (!is_succeed)
        {
            return false;
        }
    }
    return true;
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
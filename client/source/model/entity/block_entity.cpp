#include <format>

#include "model/entity/block_entity.hpp"

#define ENUM_STR(enum) #enum
std::string to_string(BlockState state)
{
    switch (state)
    {
    case BlockState::Completed:
        return ENUM_STR(BlockState::Completed);
    case BlockState::Failed:
        return ENUM_STR(BlockState::Failed);
    case BlockState::InTransfer:
        return ENUM_STR(BlockState::InTransfer);
    case BlockState::Waiting:
        return ENUM_STR(BlockState::Waiting);
    default:
        return ENUM_STR(BlockState::Unknown);
    }

}

std::string BlockEntity::to_string() const
{
    return std::format("block_id={} | block_size={} | end_time={} | offset={} | start_time={} | state={} | task_id={}",
        block_id, block_size, end_time.time_since_epoch().count(), offset, start_time.time_since_epoch().count(), ::to_string(state), task_id);
}
#include <format>

#include "model/entity/task_entity.hpp"

#define ENUM_STR(enum) #enum
std::string to_string(TaskState state)
{
    switch (state)
    {
    case TaskState::Completed:
        return ENUM_STR(TaskState::Completed);
    case TaskState::Failed:
        return ENUM_STR(TaskState::Failed);
    case TaskState::InTransfer:
        return ENUM_STR(TaskState::InTransfer);
    case TaskState::Waiting:
        return ENUM_STR(TaskState::Waiting);
    default:
        return ENUM_STR(TaskState::Unknown);
    }

}

std::string to_string(Operation operation)
{
    switch (operation)
    {
    case Operation::Download:
        return ENUM_STR(Operation::Download);
    case Operation::Upload:
        return ENUM_STR(Operation::Upload);
    default:
        return ENUM_STR(Operation::Unknown);
    }

}

std::string TaskEntity::to_string() const
{
    return std::format("end_time={} | file_id={} | operation={} | start_time={} | state={} | task_id={}",
        end_time.time_since_epoch().count(),
        file_id,
        ::to_string(operation),
        start_time.time_since_epoch().count(),
        ::to_string(state),
        task_id);
}
#include "service/task_service.hpp"

TaskService::TaskService() {}

TaskService::~TaskService() {}

void TaskService::init()
{
    m_task_repository.init();
    auto max_task_id = m_task_repository.get_max_task_id();
    if (max_task_id < 0)
    {
        max_task_id = 0;
    }
    m_task_counter.store(max_task_id + 1);
}

std::vector<TaskEntity> TaskService::get_all()
{
    return m_task_repository.get_all();
}

std::optional<TaskEntity> TaskService::get_by_task_id(int64_t task_id)
{
    return m_task_repository.get_by_task_id(task_id);
}

std::vector<TaskEntity> TaskService::get_by_file_id(int64_t file_id)
{
    return m_task_repository.get_by_file_id(file_id);
}

bool TaskService::add(TaskEntity& task_entity)
{
    task_entity.task_id = m_task_counter.fetch_add(1);
    return m_task_repository.add(task_entity);
}

bool TaskService::update(const TaskEntity& task_entity)
{
    return m_task_repository.update(task_entity);
}

bool TaskService::remove(int64_t task_id)
{
    return m_task_repository.remove(task_id);
}

int64_t TaskService::get_all_count()
{
    return m_task_repository.get_all_count();
}

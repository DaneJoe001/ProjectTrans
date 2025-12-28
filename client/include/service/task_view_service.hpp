#pragma once

#include <vector>
#include <optional>

#include "model/entity/task_entity.hpp"
#include "model/entity/client_file_entity.hpp"

struct TaskViewModel
{
    TaskEntity task_entity;
    ClientFileEntity file_entity;
    int64_t total_blocks = 0;
    int64_t completed_blocks = 0;
};

class TaskViewService
{
public:
    static TaskViewService& get_instance();
    std::vector<TaskViewModel> get_all();
    std::optional<TaskViewModel> get_by_task_id(int64_t task_id);
private:
    TaskViewService();
    ~TaskViewService();
};
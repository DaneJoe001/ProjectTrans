#pragma once

#include <chrono>

#include "client/model/common.hpp"

struct BlockRequestInfo
{
    int block_id;
    int file_id;
    std::size_t offset;
    std::size_t block_size;
    Operation operation;
    FileState state;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
};
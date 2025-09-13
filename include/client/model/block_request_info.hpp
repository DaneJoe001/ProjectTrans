#pragma once

#include <chrono>
#include <format>

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
    std::string to_string() const
    {
        return std::format("block_id: {}, file_id: {}, offset: {}, block_size: {}, operation: {}, state: {}, start_time: {}, end_time: {}",
            block_id, file_id, offset, block_size,
            static_cast<int>(operation),
            static_cast<int>(state),
            std::chrono::duration_cast<std::chrono::seconds>(start_time.time_since_epoch()).count(),
            std::chrono::duration_cast<std::chrono::seconds>(end_time.time_since_epoch()).count()
        );
    }
};
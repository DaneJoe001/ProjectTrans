#pragma once

#include <chrono>
#include <format>
#include <cstdint>

#include "client/model/common.hpp"

struct BlockRequestInfo
{
    int32_t block_id;
    int32_t file_id;
    uint32_t offset;
    uint32_t block_size;
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
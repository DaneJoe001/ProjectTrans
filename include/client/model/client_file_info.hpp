#pragma once

#include <string>
#include <chrono>
#include <format>
#include <cstdint>

#include "client/model/common.hpp"

struct ClientFileInfo
{
    int32_t file_id = -1;
    std::string saved_name;
    std::string source_path;
    std::string saved_path;
    uint32_t file_size = 0;
    Operation operation = Operation::Unknown;
    FileState state = FileState::Unknown;
    std::string md5_code;
    std::chrono::time_point<std::chrono::system_clock> create_time;
    std::chrono::time_point<std::chrono::system_clock> finished_time;
    std::string to_string() const
    {
        return std::format("file_id: {}, saved_name: {}, source_path: {}, saved_path: {}, file_size: {}, operation: {}, state: {}, md5_code: {}, create_time: {}, finished_time: {}",
            file_id, saved_name, source_path, saved_path, file_size,
            static_cast<int>(operation),
            static_cast<int>(state),
            md5_code,
            std::chrono::duration_cast<std::chrono::seconds>(create_time.time_since_epoch()).count(),
            std::chrono::duration_cast<std::chrono::seconds>(finished_time.time_since_epoch()).count()
        );
    }
};

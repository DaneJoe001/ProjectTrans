#pragma once

#include <string>
#include <chrono>

#include "client/model/common.hpp"

struct FileInfo
{
    int file_id;
    std::string saved_name;
    std::string saved_path;
    std::size_t file_size;
    Operation operation;
    FileState state;
    std::string md5_code;
    std::chrono::time_point<std::chrono::steady_clock> create_time;
    std::chrono::time_point<std::chrono::steady_clock> finished_time;
};

#pragma once

#include <string>

struct FileInfo
{
    int file_id;
    std::string saved_name;
    std::string resource_path;
    std::size_t file_size;
    std::string md5_code;
};

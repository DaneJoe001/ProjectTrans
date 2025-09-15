#pragma once

#include <string>
#include <format>
#include <cstdint>

struct ServerFileInfo
{
    int32_t file_id;
    std::string file_name;
    std::string resource_path;
    uint32_t file_size;
    std::string md5_code;
    std::string to_string() const
    {
        return std::format("file_id: {}, file_name: {}, resource_path: {}, file_size: {}, md5_code: {}",
            file_id, file_name, resource_path, file_size, md5_code);
    }
    bool operator==(const ServerFileInfo& other)const
    {
        return file_id == other.file_id &&
            file_name == other.file_name &&
            resource_path == other.resource_path &&
            file_size == other.file_size &&
            md5_code == other.md5_code;
    }
};

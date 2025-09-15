#include "server/model/server_file_info.hpp"

std::string ServerFileInfo::to_string() const
    {
        return std::format("file_id: {}, file_name: {}, resource_path: {}, file_size: {}, md5_code: {}",
            file_id, file_name, resource_path, file_size, md5_code);
    }

bool ServerFileInfo::operator==(const ServerFileInfo& other)const
    {
        return file_id == other.file_id &&
            file_name == other.file_name &&
            resource_path == other.resource_path &&
            file_size == other.file_size &&
            md5_code == other.md5_code;
    }
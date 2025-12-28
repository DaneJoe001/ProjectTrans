#include <format>

#include "model/entity/client_file_entity.hpp"

std::string ClientFileEntity::to_string() const
{
    return std::format("file_id={} | file_size={} | md5_code={} | saved_name={} | saved_path={} | source_path={}",
        file_id, file_size, md5_code, saved_name, saved_path, source_path);
}
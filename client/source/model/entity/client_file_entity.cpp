#include <format>

#include "model/entity/client_file_entity.hpp"

std::string ClientFileEntity::to_string() const
{
    return std::format("file_id={} | file_name={} | file_size={} | md5_code={}",
        file_id, file_name, file_size, md5_code);
}
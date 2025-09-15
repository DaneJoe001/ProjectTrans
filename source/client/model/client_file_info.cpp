#include "client/model/client_file_info.hpp"

std::string ClientFileInfo::to_string() const
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
#include "client/model/block_request_info.hpp"

std::string BlockRequestInfo::to_string() const
{
    return std::format("block_id: {}, file_id: {}, offset: {}, block_size: {}, operation: {}, state: {}, start_time: {}, end_time: {}",
        block_id, file_id, offset, block_size,
        static_cast<int>(operation),
        static_cast<int>(state),
        std::chrono::duration_cast<std::chrono::seconds>(start_time.time_since_epoch()).count(),
        std::chrono::duration_cast<std::chrono::seconds>(end_time.time_since_epoch()).count()
    );
}
#include <format>

#include "model/transfer/download_transfer.hpp"

std::string DownloadRequestTransfer::to_string() const
{
    return std::format("file_id={} | task_id={}", file_id, task_id);
}

std::string DownloadResponseTransfer::to_string() const
{
    return std::format("file_id={} | task_id={} | file_name={} | file_size={} | md5_code={}",
        file_id, task_id, file_name, file_size, md5_code);
}
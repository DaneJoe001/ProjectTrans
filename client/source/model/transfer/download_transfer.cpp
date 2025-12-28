#include <format>

#include "model/transfer/download_transfer.hpp"

std::string DownloadRequestTransfer::to_string() const
{
    return std::format("file_id={}", file_id);
}

std::string DownloadResponseTransfer::to_string() const
{
    return std::format("file_id={} | file_size={} | md5_code={} | saved_name={} | source_path={}",
        file_id, file_size, md5_code, saved_name, source_path);
}
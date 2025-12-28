#include <format>

#include "model/transfer/block_transfer.hpp"

std::string BlockRequestTransfer::to_string() const
{
    return std::format("block_id={} | block_size={} | file_id={} | offset={}",
        block_id, block_size, file_id, offset);
}

std::string BlockResponseTransfer::to_string() const
{
    return std::format("block_id={} | block_size={} | data_size={} | file_id={} | offset={}",
        block_id, block_size, data.size(), file_id, offset);
}
#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct BlockResponseInfo
{
    int32_t file_id;
    uint32_t block_id;
    uint32_t offset;
    uint32_t block_size;
    std::vector<uint8_t> data;
};
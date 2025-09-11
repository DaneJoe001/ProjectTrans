#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct FileInfo
{
    int file_id;
    std::string file_name;
    std::size_t file_size;
    std::string saved_path;
    std::string md5_code;
};

struct FileBlockInfo
{
    int file_id;
    int block_id;
    std::size_t block_size;
};

struct FileBlockData
{
    int file_id;
    int block_id;
    std::vector<uint8_t> data;
    std::size_t offset;
};
#pragma once

#include <cstdint>

#include "client/model/client_file_info.hpp"

struct TransInfo
{
    ClientFileInfo file_info;
    uint32_t current_count;
    uint32_t total_count;
};

struct DownloadRequest
{
    std::string file_url;
    std::string username;
    std::string password;
};
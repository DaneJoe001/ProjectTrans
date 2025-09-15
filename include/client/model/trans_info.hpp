#pragma once

#include "client/model/client_file_info.hpp"

struct TransInfo
{
    ClientFileInfo file_info;
    std::size_t current_count;
    std::size_t total_count;
};

struct DownloadRequest
{
    std::string file_url;
    std::string username;
    std::string password;
};
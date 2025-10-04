#pragma once

#include <vector>
#include <cstdint>

#include "server/model/server_file_info.hpp"
#include "server/model/block_response_info.hpp"
#include "server/service/server_file_info_service.hpp"

class MessageHandler
{
public:
    static std::vector<uint8_t> build_download_response(const ServerFileInfo& file_info);

    static std::vector<uint8_t> build_upload_response(const ServerFileInfo& file_info);

    static std::vector<uint8_t> build_test_response(const ServerFileInfo& file_info);

    static std::vector<uint8_t> build_block_response(const BlockResponseInfo& block_response_info);

    static BlockResponseInfo build_block_data(const std::vector<uint8_t>& data);

    static BlockResponseInfo build_block_data(const std::string& data);
public:
    static ServerFileInfoService m_file_info_service;
};
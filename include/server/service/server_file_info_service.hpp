#pragma once

#include <cstdint>

#include "server/repository/server_file_info_repository.hpp"

/**
 * @class FileInfoService
 * @brief 服务器端文件信息服务
 */
class ServerFileInfoService
{
public:
    std::vector<ServerFileInfo> get_all();
    bool add(const ServerFileInfo& file_info);
    std::optional<ServerFileInfo> get_by_id(int32_t file_id);
    int32_t count();
    std::optional<ServerFileInfo> get_by_md5(const std::string& md5_code);
    bool update(const ServerFileInfo& file_info);
    bool remove(int32_t file_id);
    void init();
private:
    ServerFileInfoRepository file_info_repository;
};
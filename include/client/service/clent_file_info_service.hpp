#pragma once

#include "client/repository/client_file_info_repository.hpp"

/**
 * @class FileInfoService
 * @brief 客户端文件信息服务
 */
class ClientFileInfoService
{
public:
    std::vector<ClientFileInfo> get_all();
    bool add(const ClientFileInfo& file_info);
    std::optional<ClientFileInfo> get_by_id(int file_id);
    std::optional<ClientFileInfo> get_by_md5(const std::string& md5_code);
    bool update(const ClientFileInfo& file_info);
    bool remove(int file_id);
private:
    ClientFileInfoRepository file_info_repository;
};
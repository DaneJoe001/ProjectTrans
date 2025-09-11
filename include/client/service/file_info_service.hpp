#pragma once

#include "client/repository/file_info_repository.hpp"

/**
 * @class FileInfoService
 * @brief 客户端文件信息服务
 */
class FileInfoService
{
public:
    std::vector<FileInfo> get_all();
    bool add(const FileInfo& file_info);
    std::optional<FileInfo> get_by_id(int file_id);
    bool update(const FileInfo& file_info);
    bool remove(int file_id);
private:
    FileInfoRepository file_info_repository;
};
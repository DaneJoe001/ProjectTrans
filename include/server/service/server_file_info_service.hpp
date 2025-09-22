#pragma once

/**
 * @file server_file_info_service.hpp
 * @brief 服务器文件信息服务
 * @author DaneJoe001
 */


#include <cstdint>

#include "server/repository/server_file_info_repository.hpp"

 /**
  * @class ServerFileInfoService
  * @brief 服务器端文件信息服务
  */
class ServerFileInfoService
{
public:
    /**
     * @brief 获取所有文件信息
     * @return 文件信息列表
     */
    std::vector<ServerFileInfo> get_all();
    /**
     * @brief 添加文件信息
     * @param file_info 文件信息
     * @return 是否添加成功
     */
    bool add(const ServerFileInfo& file_info);
    /**
     * @brief 获取文件信息
     * @param file_id 文件ID
     * @return 文件信息
     */
    std::optional<ServerFileInfo> get_by_id(int32_t file_id);
    /**
     * @brief 获取数量
     * @return 数量
     */
    int32_t count();
    /**
     * @brief 获取文件信息
     * @param md5_code MD5码
     * @return 文件信息
     */
    std::optional<ServerFileInfo> get_by_md5(const std::string& md5_code);
    /**
     * @brief 更新文件信息
     * @param file_info 文件信息
     * @return 是否更新成功
     */
    bool update(const ServerFileInfo& file_info);
    /**
     * @brief 删除文件信息
     * @param file_id 文件ID
     * @return 是否删除成功
     */
    bool remove(int32_t file_id);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 文件信息仓库
    ServerFileInfoRepository file_info_repository;
    /// @brief 判断是否已初始化
    bool is_init = false;
};
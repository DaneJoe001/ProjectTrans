#pragma once

/**
 * @file client_file_info_service.hpp
 * @brief 客户端文件信息服务
 * @author DaneJoe001
 */

#include <cstdint>

#include "client/repository/client_file_info_repository.hpp"

 /**
  * @class FileInfoService
  * @brief 客户端文件信息服务
  */
class ClientFileInfoService
{
public:
    /**
     * @brief 获取所有客户端文件信息
     * @return 客户端文件信息列表
     */
    std::vector<ClientFileInfo> get_all();
    std::optional<ClientFileInfo> get_by_saved_name_and_path(const std::string& saved_name, const std::string& path);
    /**
     * @brief 添加客户端文件信息
     * @param file_info 客户端文件信息
     * @return 是否成功
     */
    bool add(const ClientFileInfo& file_info);
    /**
     * @brief 获取客户端文件信息
     * @param file_id 文件ID
     * @return 客户端文件信息
     */
    std::optional<ClientFileInfo> get_by_id(int32_t file_id);
    /**
     * @brief 获取客户端文件信息
     * @param md5_code MD5码
     * @return 客户端文件信息
     */
    std::optional<ClientFileInfo> get_by_md5(const std::string& md5_code);
    /**
     * @brief 更新客户端文件信息
     * @param file_info 客户端文件信息
     * @return 是否成功
     */
    bool update(const ClientFileInfo& file_info);
    /**
     * @brief 删除客户端文件信息
     * @param file_id 文件ID
     * @return 是否成功
     */
    bool remove(int32_t file_id);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 客户端文件信息仓库
    ClientFileInfoRepository m_file_info_repository;
};
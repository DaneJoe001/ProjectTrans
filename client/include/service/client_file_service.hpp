/**
 * @file client_file_service.hpp
 * @brief 客户端文件信息服务
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <cstdint>

#include "model/entity/client_file_entity.hpp"
#include "repository/client_file_repository.hpp"

 /**
  * @class ClientFileService
  * @brief 客户端文件信息服务
  * @details 封装对 ClientFileRepository 的访问，提供客户端文件信息的增删改查接口。
  */
class ClientFileService
{
public:
    /**
     * @brief 构造函数
     */
    ClientFileService();
    /**
     * @brief 析构函数
     */
    ~ClientFileService();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取所有客户端文件信息
     * @return 客户端文件信息列表
     */
    std::vector<ClientFileEntity> get_all();
    /**
     * @brief 添加客户端文件信息
     * @param client_file_entity 客户端文件信息
     * @return 是否成功
     */
    bool add(const ClientFileEntity& client_file_entity);
    /**
     * @brief 获取客户端文件信息
     * @param file_id 文件ID
     * @return 客户端文件信息（不存在时返回空）
     */
    std::optional<ClientFileEntity> get_by_file_id(int32_t file_id);
    /**
     * @brief 获取客户端文件信息
     * @param md5_code MD5码
     * @return 客户端文件信息（不存在时返回空）
     */
    std::optional<ClientFileEntity> get_by_md5(const std::string& md5_code);
    /**
     * @brief 更新客户端文件信息
     * @param client_file_entity 客户端文件信息
     * @return 是否成功
     */
    bool update(
        const ClientFileEntity& client_file_entity);
    /**
     * @brief 删除客户端文件信息
     * @param file_id 文件ID
     * @return 是否成功
     */
    bool remove(int64_t file_id);
private:
    /// @brief 客户端文件信息仓库
    ClientFileRepository m_client_file_repository;
};
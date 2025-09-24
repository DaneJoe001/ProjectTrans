#pragma once

/**
 * @file client_file_info_repository.hpp
 * @brief 客户端文件信息仓库
 * @author DaneJoe001
 */

#include <optional>
#include <vector>
#include <cstdint>

#include "client/model/client_file_info.hpp"
#include "common/database/i_database.hpp"

 /**
  * @class ClientFileInfoRepository
  * @brief 客户端文件信息仓库
  */
class ClientFileInfoRepository {
public:
    /**
     * @brief 构造函数
     */
    ClientFileInfoRepository();
    /**
     * @brief 析构函数
     */
    ~ClientFileInfoRepository();
    /**
     * @brief 确保表存在
     * @return 是否成功
     */
    bool ensure_table_exists();
    /**
     * @brief 初始化
     */
    void init();
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
     * @brief 是否初始化
     * @return 是否初始化
     */
    bool is_init()const;
private:
    /// @brief 数据库
    std::shared_ptr<IDatabase> m_database;
};
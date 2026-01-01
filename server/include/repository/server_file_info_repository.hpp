#pragma once

/**
 * @file server_file_info_repository.hpp
 * @brief 服务器文件信息仓库
 * @author DaneJoe001
 */


#include <optional>
#include <vector>
#include <cstdint>

#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_database.hpp>

#include "model/entity/server_file_entity.hpp"

 /**
  * @class ServerFileInfoRepository
  * @brief 服务器文件信息仓库
  */
class ServerFileInfoRepository
{
public:
    /**
     * @brief 构造函数
     */
    ServerFileInfoRepository();
    /**
     * @brief 析构函数
     */
    ~ServerFileInfoRepository();
    /**
     * @brief 确保表存在
     * @return 是否存在
     */
    bool ensure_table_exists();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取数量
     * @return 数量
     */
    int32_t count();
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
     * @brief 是否初始化
     * @return 是否初始化
     */
    bool is_init()const;
private:
    /// @brief 数据库
    DaneJoe::SqlDatabasePtr m_database;
    DaneJoe::SqlQueryPtr m_query;
};
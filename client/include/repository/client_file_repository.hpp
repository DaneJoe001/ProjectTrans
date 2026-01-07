/**
  * @file client_file_repository.hpp
  * @brief 客户端文件信息仓库
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_schema.hpp>
#include <danejoe/database/sql_table_query.hpp>

#include "model/entity/client_file_entity.hpp"

  /**
    * @class ClientFileRepository
    * @brief 客户端文件信息仓库
    * @details 负责对 ClientFileEntity 进行数据库持久化读写操作。
    */
class ClientFileRepository
{
public:
    /**
     * @brief 构造函数
     */
    ClientFileRepository();
    /**
     * @brief 析构函数
     */
    ~ClientFileRepository();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 确保表存在
     * @return 是否成功
     */
    bool ensure_table_exists();
    /**
     * @brief 获取全部文件信息
     * @return 文件信息列表
     */
    std::vector<ClientFileEntity> get_all();

    /**
     * @brief 添加文件信息
     * @param file_info 文件信息
     * @return 是否成功
     */
    bool add(const ClientFileEntity& file_info);
    /**
     * @brief 通过文件ID获取文件信息
     * @param file_id 文件ID
     * @return 文件信息（不存在时返回空）
     */
    std::optional<ClientFileEntity> get_by_id(int64_t file_id);
    /**
     * @brief 通过 MD5 获取文件信息
     * @param md5_code MD5码
     * @return 文件信息（不存在时返回空）
     */
    std::optional<ClientFileEntity> get_by_md5(const std::string& md5_code);
    /**
     * @brief 更新文件信息
     * @param file_info 文件信息
     * @return 是否成功
     */
    bool update(const ClientFileEntity& file_info);
    /**
     * @brief 删除文件信息
     * @param file_id 文件ID
     * @return 是否成功
     */
    bool remove(int64_t file_id);
    /**
     * @brief 获取全部文件数量
     * @return 文件数量
     */
    int64_t get_all_count();
private:
    /**
     * @brief 将查询结果转换为文件信息实体列表
     * @param data 查询结果数据
     * @return 文件信息实体列表
     */
    std::vector<ClientFileEntity> from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 数据表信息
    std::shared_ptr<DaneJoe::SqlTableItem> m_table;
    /// @brief 数据表查询器
    DaneJoe::SqlTableQuery m_table_query;
};

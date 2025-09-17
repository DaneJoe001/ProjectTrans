#pragma once

/**
 * @file block_request_info_repository.hpp
 * @brief 块请求信息仓库
 * @author DaneJoe001
 */

#include <optional>

#include "client/model/block_request_info.hpp"
#include "common/database/i_database.hpp"

 /**
  * @class BlockRequestInfoRepository
  * @brief 块请求信息仓库
  */
class BlockRequestInfoRepository
{
public:
    /**
     * @brief 构造函数
     */
    BlockRequestInfoRepository();
    /**
     * @brief 析构函数
     */
    ~BlockRequestInfoRepository();
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
     * @brief 获取所有块请求信息
     * @return 块请求信息列表
     */
    std::vector<BlockRequestInfo> get_all();
    /**
     * @brief 添加块请求信息
     * @param block_info 块请求信息
     * @return 是否成功
     */
    bool add(const BlockRequestInfo& block_info);
    /**
     * @brief 获取块请求信息
     * @param block_id 块ID
     * @return 块请求信息
     */
    bool remove(int32_t block_id);
    std::optional<BlockRequestInfo> get_by_id(int32_t block_id);
    /**
     * @brief 更新块请求信息
     * @param block_info 块请求信息
     * @return 是否成功
     */
    bool update(const BlockRequestInfo& block_info);
    /**
     * @brief 是否初始化
     * @return 是否初始化
     */
    bool is_init()const;
private:
    /// @brief 数据库
    std::shared_ptr<IDatabase> m_database;
};
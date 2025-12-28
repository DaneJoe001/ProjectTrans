#pragma once

/**
 * @file block_request_info_repository.hpp
 * @brief 块请求信息仓库
 * @author DaneJoe001
 */

#include <optional>

#include <danejoe/database/sql_database.hpp>
#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sql_table_query.hpp>

#include "model/entity/block_entity.hpp"

 /**
  * @class BlockRepository
  * @brief 块请求信息仓库
  */
class BlockRepository
{
public:
    /**
     * @brief 构造函数
     */
    BlockRepository();
    /**
     * @brief 析构函数
     */
    ~BlockRepository();
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
     * @brief 获取所有块请求信息
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_all();
    /**
     * @brief 获取指定文件的块请求信息
     * @param file_id 文件ID
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_by_task_id(int64_t task_id);
    /**
     * @brief 获取指定文件的块请求信息
     * @param file_id 文件ID
     * @param state 状态
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 获取指定任务的块请求信息数量
     * @param task_id 任务ID
     * @param state 状态
     * @return 数量
     */
    int64_t get_count_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 添加块请求信息
     * @param block 块请求信息
     * @return 是否成功
     */
    bool add(const BlockEntity& block);
    /**
     * @brief 获取块请求信息
     * @param block_id 块ID
     * @return 块请求信息
     */
    bool remove(int64_t block_id);
    /**
     * @brief 获取块请求信息
     * @param block_id 块ID
     * @return 块请求信息
     */
    std::optional<BlockEntity> get_by_block_id(int64_t block_id);
    /**
     * @brief 更新块请求信息
     * @param block 块请求信息
     * @return 是否成功
     */
    bool update(const BlockEntity& block);
    std::vector<BlockEntity> from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data);
private:
    bool m_is_init = false;
    /// @brief 数据库
    std::shared_ptr<DaneJoe::SqlTableItem> m_table;
    DaneJoe::SqlTableQuery m_table_query;
};
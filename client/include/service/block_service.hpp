/**
 * @file block_request_info_service.hpp
 * @brief 块请求信息服务
 * @author DaneJoe001
 */
#pragma once

#include "model/entity/block_entity.hpp"
#include "repository/block_repository.hpp"
#include "model/entity/client_file_entity.hpp"

struct BlockParam
{
    int64_t block_bytes = 10 * 1024 * 1024;
};

/**
 * @class BlockService
 * @brief 块请求信息服务
 */
class BlockService
{
public:
    BlockService();
    ~BlockService();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取所有块请求信息
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_all();
    /**
     * @brief 获取指定文件的块请求信息
     * @param task_id 文件ID
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_by_task_id(int64_t task_id);
    /**
     * @brief 获取指定文件的块请求信息
     * @param task_id 文件ID
     * @param state 状态
     * @return 块请求信息列表
     */
    std::vector<BlockEntity> get_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 获取指定文件的块请求信息数量
     * @param task_id 文件ID
     * @param state 状态
     * @return 数量
     */
    int64_t get_count_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 添加块请求信息
     * @param block_entity 块请求信息
     * @return 是否成功
     */
    bool add(const BlockEntity& block_entity);
    bool add(const ClientFileEntity& file_entity, int64_t task_id, const BlockParam& param);
    /**
     * @brief 获取块请求信息
     * @param block_id 块ID
     * @return 块请求信息
     */
    std::optional<BlockEntity> get_by_id(int64_t block_id);
    /**
     * @brief 更新块请求信息
     * @param block_info 块请求信息
     * @return 是否成功
     */
    bool update(const BlockEntity& block_entity);
    /**
     * @brief 删除块请求信息
     * @param block_id 块ID
     * @return 是否成功
     */
    bool remove(int64_t block_id);
private:
    /// @brief 块请求信息仓库
    BlockRepository m_block_repository;
};
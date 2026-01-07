/**
 * @file block_service.hpp
 * @brief 块服务
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include "model/entity/block_entity.hpp"
#include "repository/block_repository.hpp"
#include "model/entity/client_file_entity.hpp"

/**
 * @struct BlockParam
 * @brief 块参数
 * @details 用于生成块列表时的分块策略参数。
 */
struct BlockParam
{
    /// @brief 分块大小（Bytes）
    int64_t block_bytes = 10 * 1024 * 1024;
};

/**
 * @class BlockService
 * @brief 块服务
 * @details 提供块信息（BlockEntity）的管理接口，并通过 BlockRepository 进行持久化。
 */
class BlockService
{
public:
    /**
     * @brief 构造函数
     */
    BlockService();
    /**
     * @brief 析构函数
     */
    ~BlockService();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取所有块信息
     * @return 块信息列表
     */
    std::vector<BlockEntity> get_all();
    /**
     * @brief 获取指定任务的块信息
     * @param task_id 任务ID
     * @return 块信息列表
     */
    std::vector<BlockEntity> get_by_task_id(int64_t task_id);
    /**
     * @brief 获取指定任务的指定状态块信息
     * @param task_id 任务ID
     * @param state 块状态
     * @return 块信息列表
     */
    std::vector<BlockEntity> get_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 获取指定任务的指定状态块数量
     * @param task_id 任务ID
     * @param state 块状态
     * @return 数量
     */
    int64_t get_count_by_task_id_and_block_state(int64_t task_id, BlockState state);
    /**
     * @brief 添加块信息
     * @param block_entity 块信息
     * @return 是否成功
     */
    bool add(const BlockEntity& block_entity);
    /**
     * @brief 为指定任务与文件生成块信息并写入仓库
     * @param file_entity 文件信息
     * @param task_id 任务ID
     * @param param 分块参数
     * @return 是否成功
     */
    bool add(const ClientFileEntity& file_entity, int64_t task_id, const BlockParam& param);
    /**
     * @brief 通过块ID获取块信息
     * @param block_id 块ID
     * @return 块信息（不存在时返回空）
     */
    std::optional<BlockEntity> get_by_id(int64_t block_id);
    /**
     * @brief 更新块信息
     * @param block_entity 块信息
     * @return 是否成功
     */
    bool update(const BlockEntity& block_entity);
    /**
     * @brief 删除块信息
     * @param block_id 块ID
     * @return 是否成功
     */
    bool remove(int64_t block_id);
private:
    /// @brief 块信息仓库
    BlockRepository m_block_repository;
};
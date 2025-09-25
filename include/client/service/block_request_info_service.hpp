#pragma once

/**
 * @file block_request_info_service.hpp
 * @brief 块请求信息服务
 * @author DaneJoe001
 */


#include "client/repository/block_request_info_repository.hpp"

 /**
  * @class BlockRequestInfoService
  * @brief 块请求信息服务
  */
class BlockRequestInfoService
{
public:
    /**
     * @brief 获取所有块请求信息
     * @return 块请求信息列表
     */
    std::vector<BlockRequestInfo> get_all();
    /**
     * @brief 获取指定文件的块请求信息
     * @param file_id 文件ID
     * @return 块请求信息列表
     */
    std::vector<BlockRequestInfo> get_by_file_id(int32_t file_id);
    /**
     * @brief 获取指定文件的块请求信息
     * @param file_id 文件ID
     * @param state 状态
     * @return 块请求信息列表
     */
    std::vector<BlockRequestInfo> get_by_file_id_and_state(int32_t file_id, FileState state);
    /**
     * @brief 获取指定文件的块请求信息数量
     * @param file_id 文件ID
     * @param state 状态
     * @return 数量
     */
    int64_t get_count_by_file_id_and_state(int32_t file_id, FileState state);
    /**
     * @brief 初始化
     */
    void init();
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
    std::optional<BlockRequestInfo> get_by_id(int32_t block_id);
    /**
     * @brief 更新块请求信息
     * @param block_info 块请求信息
     * @return 是否成功
     */
    bool update(const BlockRequestInfo& block_info);
    /**
     * @brief 删除块请求信息
     * @param block_id 块ID
     * @return 是否成功
     */
    bool remove(int32_t block_id);
private:
    /// @brief 块请求信息仓库
    BlockRequestInfoRepository m_block_request_info_repository;
};
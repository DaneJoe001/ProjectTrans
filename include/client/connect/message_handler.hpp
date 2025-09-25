#pragma once

/**
 * @file message_handler.hpp
 * @brief 消息处理类
 * @author DaneJoe001
 */

#include <string>
#include <vector>
#include <list>
#include <cstdint>

#include "common/network/url_resolver.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"

/**
 * @brief 消息处理类
 * @details 用于构建和解析消息
 */
class MessageHandler
{
public:
    /**
     * @brief 添加账号信息
     * @param raw_info 原始信息
     * @param account 账号
     * @param password 密码
     * @return 添加后的信息
     */
    static UrlResolver::UrlInfo add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password);
    /**
     * @brief 解析原始文件信息
     * @param raw_data 原始数据
     * @param operation 操作
     * @return 解析后的文件信息
     */
    static ClientFileInfo parse_raw_file_info(const std::string& raw_data, Operation operation);
    /**
     * @brief 计算分块数据信息
     * @param file_info 文件信息
     * @param config 块参数配置
     * @return 计算后的块请求信息列表
     */
    static std::list<BlockRequestInfo> calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config);
    /**
     * @brief 构建块请求
     * @param block_request_info 块请求信息
     * @return 构建后的块请求
     * @todo 实现信息序列化
     */
    static std::string build_block_request(const BlockRequestInfo& block_request_info);
};
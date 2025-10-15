#pragma once

/**
 * @file message_handler.hpp
 * @brief 消息处理类
 * @author DaneJoe001
 */

/// @todo  messageType改为路径之上的GET/POST类型
/// @todo path存储原始的/download之类的路径

#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <map>

#include "common/protocol/danejoe_protocol.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"

namespace Client
{
  /**
  * @brief 消息处理类
  * @details 用于构建和解析消息
  */
class MessageHandler
{
public:
    
public:
    /**
     * @brief 计算分块数据信息
     * @param file_info 文件信息
     * @param config 块参数配置
     * @return 计算后的块请求信息列表
     */
    static std::list<BlockRequestInfo> calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config);
    /**
     * @brief 解析响应信息
     * @param data 数据
     * @note 确保数据为完整的数据帧
     * @return 解析后的消息信息
     */
    static std::optional<DaneJoe::Protocol::RequestInfo> parse_response(const std::vector<uint8_t>& data);
    /**
     * @brief 解析下载响应
     * @param body 消息体
     * @return 解析后的文件信息
     */
    static std::optional<ClientFileInfo> parse_download_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析块响应
     * @param body 消息体
     * @todo 实现信息解析
     */
    static std::optional<BlockResponseInfo> parse_block_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析测试响应
     * @param body 消息体
     * @return 解析后的测试信息
     */
    static std::string parse_test_response(const std::vector<uint8_t>& body);
    /**
     * @brief 构建请求信息
     * @param protocol 协议
     * @param info 消息
     * @return 序列化的消息
     */
    static std::vector<uint8_t> build_request(DaneJoe::Protocol::RequestInfo info);
    /**
     * @brief 构建测试消息
     * @param message 消息
     */
    static std::vector<uint8_t> build_test_request(const UrlResolver::UrlInfo url_info,const std::string& message);
    /**
     * @brief 构建上传消息
     * @param file_info 文件信息
     * @return 构建后的上传消息
     */
    static std::vector<uint8_t> build_upload_request(const UrlResolver::UrlInfo url_info,const ClientFileInfo& file_info);
    /**
     * @brief 构建下载消息
     * @param path 下载路径
     * @return 构建后的下载消息
     */
    static std::vector<uint8_t> build_download_request(const UrlResolver::UrlInfo url_info,const std::string& account,const std::string& password);
    /**
     * @brief 构建块请求
     * @param block_request_info 块请求信息
     * @return 构建后的块请求
     */
    static std::vector<uint8_t> build_block_request(const UrlResolver::UrlInfo url_info,const BlockRequestInfo& block_request_info);
};
}
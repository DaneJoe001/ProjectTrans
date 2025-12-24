#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include "danejoe/network/url/url_info.hpp"

#include "common/protocol/danejoe_protocol.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"

class ClientMessageCodec
{
public:
    /**
     * @brief 解析响应信息
     * @param data 数据
     * @note 确保数据为完整的数据帧
     * @return 解析后的消息信息
     */
    std::optional<DaneJoe::Protocol::RequestInfo> parse_response(const std::vector<uint8_t>& data);
    /**
     * @brief 解析下载响应
     * @param body 消息体
     * @return 解析后的文件信息
     */
    std::optional<ClientFileInfo> parse_download_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析块响应
     * @param body 消息体
     * @todo 实现信息解析
     */
    std::optional<BlockResponseInfo> parse_block_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析测试响应
     * @param body 消息体
     * @return 解析后的测试信息
     */
    std::string parse_test_response(const std::vector<uint8_t>& body);
    /**
     * @brief 构建请求信息
     * @param protocol 协议
     * @param info 消息
     * @return 序列化的消息
     */
    std::vector<uint8_t> build_request(DaneJoe::Protocol::RequestInfo info);
    /**
     * @brief 构建测试消息
     * @param message 消息
     */
    std::vector<uint8_t> build_test_request(const DaneJoe::UrlInfo url_info, const std::string& message);
    /**
     * @brief 构建上传消息
     * @param file_info 文件信息
     * @return 构建后的上传消息
     */
    std::vector<uint8_t> build_upload_request(const DaneJoe::UrlInfo url_info, const ClientFileInfo& file_info);
    /**
     * @brief 构建下载消息
     * @param path 下载路径
     * @return 构建后的下载消息
     */
    std::vector<uint8_t> build_download_request(const DaneJoe::UrlInfo url_info, const std::string& account, const std::string& password);
    /**
     * @brief 构建块请求
     * @param block_request_info 块请求信息
     * @return 构建后的块请求
     */
    std::vector<uint8_t> build_block_request(const DaneJoe::UrlInfo url_info, const BlockRequestInfo& block_request_info);
};
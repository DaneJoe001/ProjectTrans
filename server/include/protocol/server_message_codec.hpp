/**
 * @file server_message_codec.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 服务端消息编解码器
 * @date 2026-01-06
 */

#pragma once

#include <optional>

#include "model/transfer/envelope_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/test_transfer.hpp"

/**
 * @class ServerMessageCodec
 * @brief 服务端消息编解码器
 */
class ServerMessageCodec
{
public:
    /**
     * @brief 解析信封请求
     * @param data 输入字节数组
     * @return 解析成功返回请求对象，否则返回空
     */
    std::optional<EnvelopeRequestTransfer> try_parse_byte_array_request(const std::vector<uint8_t>& data);
    /**
     * @brief 解析块请求
     * @param data 输入字节数组
     * @return 解析成功返回请求对象，否则返回空
     */
    std::optional<BlockRequestTransfer> try_parse_byte_array_block_request(const std::vector<uint8_t>& data);
    /**
     * @brief 解析下载请求
     * @param data 输入字节数组
     * @return 解析成功返回请求对象，否则返回空
     */
    std::optional<DownloadRequestTransfer> try_parse_byte_array_download_request(const std::vector<uint8_t>& data);
    /**
     * @brief 解析测试请求
     * @param data 输入字节数组
     * @return 解析成功返回请求对象，否则返回空
     */
    std::optional<TestRequestTransfer> try_parse_byte_array_test_request(const std::vector<uint8_t>& data);
    /**
     * @brief 构建信封响应字节数组
     * @param response 信封响应
     * @return 可发送的响应字节数组
     */
    std::vector<uint8_t> build_response_byte_array(const EnvelopeResponseTransfer& response);
    /**
     * @brief 构建块响应字节数组
     * @param block_response 块响应
     * @param request_id 请求ID
     * @return 可发送的响应字节数组
     */
    std::vector<uint8_t> build_block_response_byte_array(const BlockResponseTransfer& block_response, int64_t request_id);
    /**
     * @brief 构建下载响应字节数组
     * @param download_response 下载响应
     * @param request_id 请求ID
     * @return 可发送的响应字节数组
     */
    std::vector<uint8_t> build_download_response_byte_array(const DownloadResponseTransfer& download_response, int64_t request_id);
    /**
     * @brief 构建测试响应字节数组
     * @param block_response 测试响应
     * @param request_id 请求ID
     * @return 可发送的响应字节数组
     */
    std::vector<uint8_t> build_test_response_byte_array(const TestResponseTransfer& block_response, int64_t request_id);
private:

};
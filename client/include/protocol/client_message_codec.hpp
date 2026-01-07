/**
  * @file client_message_codec.hpp
  * @brief 客户端消息编解码器
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "model/transfer/envelope_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/test_transfer.hpp"

  /**
   * @class ClientMessageCodec
   * @brief 客户端消息编解码器
   * @details 负责：
   *          - 将请求传输对象序列化为可发送的字节数组（帧）
   *          - 将响应帧/消息体反序列化为对应的传输对象
   */
class ClientMessageCodec
{
public:
    /**
     * @brief 解析响应信息
     * @param data 数据
     * @note 确保数据为完整的数据帧
     * @return 解析后的消息信息
     */
    std::optional<EnvelopeResponseTransfer> try_parse_byte_array_response(const std::vector<uint8_t>& data);
    /**
     * @brief 解析下载响应
     * @param body 消息体
     * @return 解析后的文件信息
     */
    std::optional<DownloadResponseTransfer> try_parse_byte_array_download_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析块响应
     * @param body 消息体
     * @return 解析后的块响应
     * @todo 实现信息解析
     */
    std::optional<BlockResponseTransfer> try_parse_byte_array_block_response(const std::vector<uint8_t>& body);
    /**
     * @brief 解析测试响应
     * @param body 消息体
     * @return 解析后的测试信息
     */
    std::optional<TestResponseTransfer> try_parse_byte_array_test_response(const std::vector<uint8_t>& body);
    /**
     * @brief 构建请求信息
     * @param request 请求信封对象
     * @return 序列化的消息
     */
    std::vector<uint8_t> build_request_byte_array(EnvelopeRequestTransfer request);
    /**
     * @brief 构建测试消息
     * @param test_request 测试请求
     * @param request_id 请求ID
     * @return 序列化的测试请求帧数据
     */
    std::vector<uint8_t> build_test_request_byte_array(
        const TestRequestTransfer& test_request,
        int64_t request_id);
    /**
     * @brief 构建下载消息
     * @param download_request 下载请求
     * @param request_id 请求ID
     * @return 构建后的下载消息
     */
    std::vector<uint8_t> build_download_request_byte_array(
        const DownloadRequestTransfer& download_request,
        int64_t request_id);
    /**
     * @brief 构建块请求
     * @param block_request 块请求
     * @param request_id 请求ID
     * @return 构建后的块请求
     */
    std::vector<uint8_t> build_block_request_byte_array(
        const BlockRequestTransfer& block_request,
        int64_t request_id);
};
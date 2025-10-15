#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>

#include "server/model/server_file_info.hpp"
#include "server/model/block_response_info.hpp"
#include "server/service/server_file_info_service.hpp"
#include "common/network/danejoe_serializer.hpp"
#include "common/protocol/danejoe_protocol.hpp"

namespace Server
{
    /**
    * @brief 消息处理类
    * @details 用于构建和解析消息
    */
    class MessageHandler
    {
    public:
    public:
        static std::vector<uint8_t> build_response(DaneJoe::Protocol::ResponseInfo info);
        /**
         * @brief 构建下载响应
         * @param file_info 文件信息
         * @return 下载响应数据(序列化的文件信息字节流)
         */
        static std::vector<uint8_t> build_download_response(const ServerFileInfo& file_info);
        /**
         * @brief 构建上传响应
         * @param file_info 文件信息
         * @return 上传响应数据(序列化的确认信息字节流)
         */
        static std::vector<uint8_t> build_upload_response(const ServerFileInfo& file_info);
        /**
         * @brief 构建测试响应
         * @param message 测试信息
         * @return 测试响应数据(序列化的测试信息字节流)
         */
        static std::vector<uint8_t> build_test_response(const std::string& message);
        /**
         * @brief 构建块响应
         * @param block_response_info 块响应信息
         * @return 块响应数据(序列化的块响应字节流)
         */
        static std::vector<uint8_t> build_block_response(const BlockResponseInfo& block_response_info);
        /**
         * @brief 解析请求,获取请求类型和消息体
         * @note 确保传入的数据为完整的数据帧
         * @param data 请求数据
         * @return 请求信息
         */
        static std::optional<DaneJoe::Protocol::RequestInfo> parse_request(const std::vector<uint8_t>& data);
        /**
         * @brief 解析块请求消息体
         * @param data 块请求数据
         */
        static std::optional<BlockResponseInfo> parse_block_request(const std::vector<uint8_t>& body);
        /**
         * @brief 解析块请求消息体
         * @param data 块请求数据
         */
        static std::optional<BlockResponseInfo> parse_block_request(const std::string& body);
        /**
         * @brief 解析下载请求消息体
         * @param data 下载请求数据
         */
        static std::optional<ServerFileInfo> parse_download_request(const std::vector<uint8_t>& body);
        /**
         * @brief 解析测试请求消息体
         * @param data 测试请求数据
         */
        static std::string parse_test_request(const std::vector<uint8_t>& body);
        /**
         * @brief 解析上传请求消息体
         * @param data 上传请求数据
         */
        static std::optional<ServerFileInfo> parse_upload_request(const std::vector<uint8_t>& body);
    public:
        /// @brief 文件信息服务
        static ServerFileInfoService m_file_info_service;
    };

}
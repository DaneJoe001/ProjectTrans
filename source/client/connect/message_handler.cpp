#include <cstdio>
#include <string>
#include <chrono>

#include "client/connect/message_handler.hpp"
#include "common/log/manage_logger.hpp"
#include "common/network/danejoe_serializer.hpp"
#include "common/protocol/danejoe_protocol.hpp"
#include "common/util/print_util.hpp"

std::list<BlockRequestInfo> Client::MessageHandler::calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config)
{
    /// @todo 根据硬件以及手动设置，来完成更具体地请求分块
    std::list<BlockRequestInfo> result_info;
    // 当前仅根据块最小大小来计算
    uint32_t block_size = config.min_block_size;
    uint32_t current_index = 0;
    while (current_index < file_info.file_size)
    {
        BlockRequestInfo block_info;
        block_info.file_id = file_info.file_id;
        block_info.offset = current_index;
        block_info.block_size = std::min(block_size, file_info.file_size - current_index);
        block_info.state = FileState::Waiting;
        block_info.operation = file_info.operation;
        result_info.push_back(block_info);
        current_index += block_info.block_size;
    }
    return result_info;
}

std::optional<ClientFileInfo> Client::MessageHandler::parse_download_response(const std::vector<uint8_t>& body)
{
    /// @todo 添加文件名长度限制/移除硬编码数值/添加长度信息(序列化添加)
    DANEJOE_LOG_TRACE("default", "MessageHandler", "Parse file info");
    ClientFileInfo info;
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(body);
    auto file_id_field_opt = serializer.get_parsed_field("file_id");
    auto file_name_field_opt = serializer.get_parsed_field("file_name");
    auto file_size_field_opt = serializer.get_parsed_field("file_size");
    auto md5_code_field_opt = serializer.get_parsed_field("md5_code");

    if (file_id_field_opt.has_value())
    {
        auto file_id_it = DaneJoe::to_value<int32_t>(file_id_field_opt.value());
        if (file_id_it.has_value())
        {
            info.file_id = file_id_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
            return ClientFileInfo{};
        }
    }
    if (file_name_field_opt.has_value())
    {
        // 默认的保存名
        info.saved_name = DaneJoe::to_string(file_name_field_opt.value());
    }
    if (file_size_field_opt.has_value())
    {
        auto file_size_it = DaneJoe::to_value<uint32_t>(file_size_field_opt.value());
        if (file_size_it.has_value())
        {
            info.file_size = file_size_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "File size parse failed");
            return ClientFileInfo{};
        }
    }
    if (md5_code_field_opt.has_value())
    {
        info.md5_code = DaneJoe::to_string(md5_code_field_opt.value());
    }
    // 初始化文件状态
    info.operation = Operation::Download;
    info.state = FileState::Waiting;
    info.create_time = std::chrono::system_clock::now();
    return info;
}

std::optional<BlockResponseInfo> Client::MessageHandler::parse_block_response(const std::vector<uint8_t>& body)
{
    BlockResponseInfo info;
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(body);

    auto block_id_field_op = serializer.get_parsed_field("block_id");
    auto file_id_field_op = serializer.get_parsed_field("file_id");
    auto offset_field_op = serializer.get_parsed_field("offset");
    auto block_size_field_op = serializer.get_parsed_field("block_size");
    auto data_field_op = serializer.get_parsed_field("data");
    if (!block_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block id parse failed");
        return std::nullopt;
    }
    auto block_id_op = DaneJoe::to_value<int32_t>(block_id_field_op.value());
    if (!block_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block id parse failed");
        return std::nullopt;
    }
    info.block_id = block_id_op.value();
    if (!file_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
        return std::nullopt;
    }
    auto file_id_op = DaneJoe::to_value<int32_t>(file_id_field_op.value());
    if (!file_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
        return std::nullopt;
    }
    info.file_id = file_id_op.value();
    if (!offset_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Offset parse failed");
        return std::nullopt;
    }
    auto offset_op = DaneJoe::to_value<uint32_t>(offset_field_op.value());
    if (!offset_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Offset parse failed");
        return std::nullopt;
    }
    info.offset = offset_op.value();
    if (!block_size_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block size parse failed");
        return std::nullopt;
    }
    auto block_size_op = DaneJoe::to_value<uint32_t>(block_size_field_op.value());
    if (!block_size_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block size parse failed");
        return std::nullopt;
    }
    info.block_size = block_size_op.value();
    if (!data_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Data parse failed");
        return std::nullopt;
    }
    info.data = DaneJoe::to_array<uint8_t>(data_field_op.value());
    return info;
}

std::string Client::MessageHandler::parse_test_response(const std::vector<uint8_t>& body)
{
    DaneJoe::DaneJoeSerializer serializer;
    auto header_opt = serializer.get_message_header(body);
    DANEJOE_LOG_DEBUG("default", "Client::MessageHandler", "Header: {}", header_opt.has_value() ? header_opt->to_string() : "Invalid header");
    serializer.deserialize(body);
    auto message_field_op = serializer.get_parsed_field("message");
    if (!message_field_op.has_value())
    {
        DANEJOE_LOG_WARN("default", "MessageHandler", "Failed to handle Test response: message field not found");
        return "";
    }
    return DaneJoe::to_string(message_field_op.value());
}

std::optional<DaneJoe::Protocol::RequestInfo> Client::MessageHandler::parse_response(const std::vector<uint8_t>& data)
{
    /// @todo 请求和响应格式不应该一致，当前应该是服务端解析逻辑AQS
    /// @todo 区分Http协议和自定义协议
    DaneJoe::Protocol::RequestInfo info;
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(data);
    auto parsed_map = serializer.get_parsed_data_map();
    for(const auto& [key, value]: parsed_map)
    {
        DANEJOE_LOG_TRACE("default", "MessageHandler", "Key: {}, Value: {}", key, DaneJoe::to_string(value));
        if(key == "type")
        {
            info.type = DaneJoe::to_request_type(DaneJoe::to_string(value));
        }
        else if (key == "body")
        {
            info.body = DaneJoe::to_array<uint8_t>(value);
        }
        else if (key == "protocol")
        {
            info.url_info.protocol = UrlResolver::to_protocol(DaneJoe::to_string(value));
        }
        else if (key == "host")
        {
            info.url_info.host = DaneJoe::to_string(value);
        }
        else if (key == "port")
        {
            auto port_op = DaneJoe::to_value<uint16_t>(value);
            if (port_op.has_value())
            {
                info.url_info.port = port_op.value();
            }
        }
        else if (key == "path")
        {
            info.url_info.path = DaneJoe::to_string(value);
        }
        else
        {
            // 其他均作为查询参数
            info.url_info.query.insert({key, DaneJoe::to_string(value)});
        }
    }
    return info;
}

std::vector<uint8_t> Client::MessageHandler::build_request(DaneJoe::Protocol::RequestInfo info)
{
    if(info.url_info.protocol == UrlResolver::UrlProtocol::Danejoe)
    {
        DaneJoe::DaneJoeSerializer serializer;
        std::string type_str =  DaneJoe::to_string(info.type);
        // 序列化消息类型
        serializer.serialize(type_str, "type");
        // 序列化消息体
        serializer.serialize(info.body, "body");
        serializer.serialize(UrlResolver::to_string(info.url_info.protocol), "protocol");
        serializer.serialize(info.url_info.host, "host");
        serializer.serialize(info.url_info.port, "port");
        serializer.serialize(info.url_info.path, "path");
        for (auto& [key, value] : info.url_info.query)
        {
            serializer.serialize(value, key);
        }
        // 序列化Url信息结构体
        // DaneJoe::DaneJoeSerializer url_info_serializer;
        // 序列化Url信息
        // serializer.serialize(url_info_serializer.get_serialized_data_vector_build(), "url_info");
        return serializer.get_serialized_data_vector_build();
    }
    else if (info.url_info.protocol == UrlResolver::UrlProtocol::Http)
    {
        std::string body_str= std::string(info.body.begin(), info.body.end());
        std::string request_str = "POST / HTTP/1.1\r\nHost: " + info.url_info.host + ":" + std::to_string(info.url_info.port) + "\r\nContent-Length: " + std::to_string(info.body.size()) + "\r\n\r\n" + body_str;
        return std::vector<uint8_t>(request_str.begin(), request_str.end());
    }
    else
    {
        return std::vector<uint8_t>();
    }
}

std::vector<uint8_t> Client::MessageHandler::build_test_request(const UrlResolver::UrlInfo url_info,const std::string& message)
{
    // 构建消息体
    DaneJoe::DaneJoeSerializer serializer;
    serializer.serialize(message, "message");
    // 构建消息
    DaneJoe::Protocol::RequestInfo info;
    info.type = DaneJoe::Protocol::RequestType::Post;
    info.url_info = url_info;
    info.url_info.path = "/test";
    info.body = serializer.get_serialized_data_vector_build();

    // 当前默认使用自定义协议
    return build_request(info);
}

std::vector<uint8_t> Client::MessageHandler::build_block_request(const UrlResolver::UrlInfo url_info,const BlockRequestInfo& block_request_info)
{
    DANEJOE_LOG_TRACE("default", "Client::MessageHandler", "Building block request for block: {}", block_request_info.to_string());
    DaneJoe::DaneJoeSerializer serializer;
    serializer.serialize(block_request_info.block_id, "block_id");
    serializer.serialize(block_request_info.file_id, "file_id");
    serializer.serialize(block_request_info.offset, "offset");
    serializer.serialize(block_request_info.block_size, "block_size");
    DaneJoe::Protocol::RequestInfo info;
    info.type = DaneJoe::Protocol::RequestType::Get;
    info.url_info = url_info;
    /// @note 当前只是自定义协议
    info.url_info.path = "/block";
    info.body = serializer.get_serialized_data_vector_build();
    // 当前默认使用自定义协议
    return build_request(info);
}

std::vector<uint8_t> Client::MessageHandler::build_download_request(const UrlResolver::UrlInfo url_info, const std::string& account, const std::string& password)
{
    DANEJOE_LOG_TRACE("default", "Client::MessageHandler", "Query params: {}", DaneJoe::to_string(url_info.query));
    // 构建消息体
    DaneJoe::DaneJoeSerializer serializer;
    /// @todo 解析url,获取文件id
    serializer.serialize(account, "account");
    serializer.serialize(password, "password");
    for(const auto& [key, value]: url_info.query)
    {
        serializer.serialize(value, key);
    }
    /// 处理协议
    DaneJoe::Protocol::RequestInfo info;
    info.type = DaneJoe::Protocol::RequestType::Get;
    info.url_info = url_info;
    info.body = serializer.get_serialized_data_vector_build();
    return build_request(info);
}
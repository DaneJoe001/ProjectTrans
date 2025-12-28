#include <fstream>

#include <danejoe/logger/logger_manager.hpp>

#include "danejoe/network/codec/serialize_codec.hpp"
#include "danejoe/network/url/url_resolver.hpp"

#include "common/protocol/danejoe_protocol.hpp"
#include "connect/message_handler.hpp"

/// @todo 由type区分转为由path区分

ServerFileInfoService Server::MessageHandler::m_file_info_service;

std::vector<uint8_t> Server::MessageHandler::build_response(DaneJoe::Protocol::ResponseInfo info)
{
    DANEJOE_LOG_DEBUG("default", "MessageHandler", "Building response with status: {}, content_type: {}, body size: {}",
        DaneJoe::to_string(info.status),
        DaneJoe::to_string(info.content_type),
        info.body.size());
    DaneJoe::SerializeCodec serializer;
    serializer.
        serialize(DaneJoe::to_code(info.status), "status").
        serialize(DaneJoe::to_code(info.content_type), "content_type").
        serialize(info.body, "body");

    auto header_opt = serializer.get_message_header(info.body);
    DANEJOE_LOG_DEBUG("default", "Server::MessageHandler", "Header: {}", header_opt.has_value() ? header_opt->to_string() : "Invalid header");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> Server::MessageHandler::build_download_response(const ServerFileInfo& file_info)
{
    /// @todo 添加协议信息
    DaneJoe::SerializeCodec serializer;
    serializer.
        serialize("download_response", "response_type").
        serialize(file_info.file_id, "file_id").
        serialize(file_info.file_name, "file_name").
        serialize(file_info.file_size, "file_size").
        serialize(file_info.md5_code, "md5_code");
    // 构建响应
    DaneJoe::Protocol::ResponseInfo info;
    info.status = DaneJoe::Protocol::ResponseStatus::Ok;
    info.content_type = DaneJoe::Protocol::ContentType::DaneJoe;
    info.body = serializer.get_serialized_data_vector_build();
    return build_response(info);
}

std::vector<uint8_t> Server::MessageHandler::build_upload_response(const ServerFileInfo& file_info)
{
    DaneJoe::SerializeCodec serializer;
    serializer.
        serialize("upload_response", "response_type").
        serialize("Received Upload request", "message");
    // 构建响应
    DaneJoe::Protocol::ResponseInfo info;
    info.status = DaneJoe::Protocol::ResponseStatus::Ok;
    info.content_type = DaneJoe::Protocol::ContentType::DaneJoe;
    info.body = serializer.get_serialized_data_vector_build();
    return build_response(info);
}

std::vector<uint8_t> Server::MessageHandler::build_test_response(const std::string& message)
{
    DaneJoe::SerializeCodec serializer;
    serializer.
        serialize("test_response", "response_type").
        serialize(message, "message");
    // 构建响应
    DaneJoe::Protocol::ResponseInfo info;
    info.status = DaneJoe::Protocol::ResponseStatus::Ok;
    info.content_type = DaneJoe::Protocol::ContentType::DaneJoe;
    info.body = serializer.get_serialized_data_vector_build();
    DANEJOE_LOG_DEBUG("default", "MessageHandler", "Info body size: {}", info.body.size());
    serializer.deserialize(info.body);
    auto map = serializer.get_parsed_data_map();
    for (const auto& [key, value] : map)
    {
        DANEJOE_LOG_TRACE("default", "MessageHandler", "Key: {}, Value: {}", key, value.to_string());
    }
    return build_response(info);
}

std::vector<uint8_t> Server::MessageHandler::build_block_response(const BlockResponseInfo& block_response_info)
{
    DaneJoe::SerializeCodec serializer;
    serializer.
        serialize("block_response", "response_type").
        serialize(block_response_info.block_id, "block_id").
        serialize(block_response_info.data, "data").
        serialize(block_response_info.file_id, "file_id").
        serialize(block_response_info.block_size, "block_size").
        serialize(block_response_info.offset, "offset");
    // 构建响应
    DaneJoe::Protocol::ResponseInfo info;
    info.status = DaneJoe::Protocol::ResponseStatus::Ok;
    info.content_type = DaneJoe::Protocol::ContentType::DaneJoe;
    info.body = serializer.get_serialized_data_vector_build();
    return build_response(info);
}

std::optional<DaneJoe::Protocol::RequestInfo> Server::MessageHandler::parse_request(const std::vector<uint8_t>& data)
{
    DaneJoe::SerializeCodec serializer;
    DaneJoe::Protocol::RequestInfo request_info;
    serializer.deserialize(data);
    auto field_map = serializer.get_parsed_data_map();
    for (const auto& [key, field] : field_map)
    {
        // DANEJOE_LOG_TRACE("default", "Server::MessageHandler", "Parsed field: {} with length: {}", key, field.value_length);
        if (key == "type")
        {
            request_info.type = DaneJoe::to_request_type(DaneJoe::to_string(field));
        }
        else if (key == "body")
        {
            request_info.body = DaneJoe::to_array<uint8_t>(field);
        }
        else if (key == "protocol")
        {
            request_info.url_info.protocol = DaneJoe::UrlResolver::to_protocol(DaneJoe::to_string(field));
        }
        else if (key == "host")
        {
            request_info.url_info.host = DaneJoe::to_string(field);
        }
        else if (key == "port")
        {
            auto port_op = DaneJoe::to_value<uint16_t>(field);
            if (port_op.has_value())
            {
                request_info.url_info.port = port_op.value();
            }
        }
        else if (key == "path")
        {
            request_info.url_info.path = DaneJoe::to_string(field);
        }
        else
        {
            // 其他均作为查询参数
            request_info.url_info.query.insert({ key, DaneJoe::to_string(field) });
        }
    }
    return request_info;
}

std::optional<BlockResponseInfo> Server::MessageHandler::parse_block_request(const std::string& body)
{
    std::vector<uint8_t> bytes = std::vector<uint8_t>(body.begin(), body.end());
    return parse_block_request(bytes);
}

std::optional<BlockResponseInfo> Server::MessageHandler::parse_block_request(const std::vector<uint8_t>& body)
{
    BlockResponseInfo block_response_info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(body);

    auto map = serializer.get_parsed_data_map();
    auto file_id_field_opt = serializer.get_parsed_field("file_id");
    auto block_id_field_opt = serializer.get_parsed_field("block_id");
    auto offset_field_opt = serializer.get_parsed_field("offset");
    auto block_size_field_opt = serializer.get_parsed_field("block_size");
    if (file_id_field_opt.has_value())
    {
        auto file_id_opt = to_value<int32_t>(file_id_field_opt.value());
        if (file_id_opt.has_value())
        {
            block_response_info.file_id = file_id_opt.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "Server::MessageHandler", "file_id is not found");
            return BlockResponseInfo();
        }
    }
    if (block_id_field_opt.has_value())
    {
        auto block_id_it = to_value<int32_t>(block_id_field_opt.value());
        if (block_id_it)
        {
            block_response_info.block_id = block_id_it.value();
        }
        else
        {
            // block_id不影响实际传输
            DANEJOE_LOG_WARN("default", "Server::MessageHandler", "block_id is not found");
        }
    }
    if (offset_field_opt.has_value())
    {
        auto offset_it = to_value<uint32_t>(offset_field_opt.value());
        if (offset_it)
        {
            block_response_info.offset = offset_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "Server::MessageHandler", "offset is not found");
            return BlockResponseInfo();
        }
    }
    if (block_size_field_opt.has_value())
    {
        auto block_size_it = to_value<uint32_t>(block_size_field_opt.value());
        if (block_size_it)
        {
            block_response_info.block_size = block_size_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "Server::MessageHandler", "block_size is not found");
            return BlockResponseInfo();
        }
    }
    m_file_info_service.init();
    auto server_file_info_op = m_file_info_service.get_by_id(block_response_info.file_id);
    if (!server_file_info_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "Server::MessageHandler", "file_id is not found");
        return BlockResponseInfo();
    }
    auto server_file_info = server_file_info_op.value();
    block_response_info.data.resize(block_response_info.block_size);
    std::ifstream fin(server_file_info.resource_path, std::ios::binary);
    fin.seekg(block_response_info.offset);
    fin.readsome(reinterpret_cast<char*>(block_response_info.data.data()), block_response_info.block_size);
    return block_response_info;
}

std::optional<ServerFileInfo> Server::MessageHandler::parse_download_request(const std::vector<uint8_t>& body)
{
    ServerFileInfo file_info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(body);
    // 通过id获取文件信息
    // 注意当前的file_id是以字符串的形式传过来的
    auto file_id_field_opt = serializer.get_parsed_field("file_id");
    if (file_id_field_opt.has_value())
    {
        auto file_id_opt = DaneJoe::to_string(file_id_field_opt.value());
        int32_t file_id = std::stoi(file_id_opt);
        // auto file_id_opt =
        //     DaneJoe::to_value<int32_t>(file_id_field_opt.value());
        // if (!file_id_opt.has_value()) {
        //     return std::nullopt;
        // }
        // int32_t file_id = file_id_opt.value();
        if (file_id <= 0)
        {
            DANEJOE_LOG_WARN("default", "Server::MessageHandler", "Failed to handle download request: invalid file id {}", file_id);
            return std::nullopt;
        }
        m_file_info_service.init();
        auto file_info_optional = m_file_info_service.get_by_id(file_id);
        DANEJOE_LOG_DEBUG("default", "-----------", "File info: {}", file_id);
        if (!file_info_optional.has_value())
        {
            DANEJOE_LOG_WARN("default", "Server::MessageHandler", "Failed to handle download request: file info not found");
            return std::nullopt;
        }
        DANEJOE_LOG_DEBUG("default", "-----------", "File info: {}", file_info_optional.value().to_string());
        file_info = file_info_optional.value();
    }
    else
    {
        DANEJOE_LOG_WARN("default", "Server::MessageHandler", "Failed to handle download request: file_id field not found");
        return std::nullopt;
    }
    return file_info;
}

std::string Server::MessageHandler::parse_test_request(const std::vector<uint8_t>& body)
{
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(body);
    auto message_field_opt = serializer.get_parsed_field("message");
    if (!message_field_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "Server::MessageHandler", "Failed to handle Test request: message field not found");
        return "";
    }
    return DaneJoe::to_string(message_field_opt.value());
}

std::optional<ServerFileInfo> Server::MessageHandler::parse_upload_request(const std::vector<uint8_t>& body)
{
    return std::nullopt;
}
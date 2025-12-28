#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/network/codec/serialize_codec.hpp"
#include <danejoe/stringify//stringify_to_string.hpp>

#include "protocol/client_message_codec.hpp"

std::optional<EnvelopeResponseTransfer> ClientMessageCodec::try_parse_response(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ClientMessageCodec", "Parsing envelope response");
    EnvelopeResponseTransfer envelope;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(data);

    auto version_field_opt = serializer.get_parsed_field("version");
    auto request_id_field_opt = serializer.get_parsed_field("request_id");
    auto status_field_opt = serializer.get_parsed_field("status");
    auto content_type_field_opt = serializer.get_parsed_field("content_type");
    auto body_field_opt = serializer.get_parsed_field("body");

    if (version_field_opt.has_value())
    {
        auto version_op = DaneJoe::to_value<uint16_t>(version_field_opt.value());
        if (version_op.has_value())
        {
            envelope.version = version_op.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Version parse failed");
            return std::nullopt;
        }
    }

    if (request_id_field_opt.has_value())
    {
        auto request_id_op = DaneJoe::to_value<uint64_t>(request_id_field_opt.value());
        if (request_id_op.has_value())
        {
            envelope.request_id = request_id_op.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Request id parse failed");
            return std::nullopt;
        }
    }

    if (status_field_opt.has_value())
    {
        auto status_op = DaneJoe::to_value<uint16_t>(status_field_opt.value());
        if (status_op.has_value())
        {
            envelope.status = static_cast<ResponseStatus>(status_op.value());
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Status parse failed");
            return std::nullopt;
        }
    }

    if (content_type_field_opt.has_value())
    {
        auto content_type_op = DaneJoe::to_value<uint8_t>(content_type_field_opt.value());
        if (content_type_op.has_value())
        {
            envelope.content_type = static_cast<ContentType>(content_type_op.value());
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Content type parse failed");
            return std::nullopt;
        }
    }

    if (body_field_opt.has_value())
    {
        envelope.body = DaneJoe::to_array<uint8_t>(body_field_opt.value());
    }

    return envelope;
}

std::optional<DownloadResponseTransfer> ClientMessageCodec::try_parse_download_response(const std::vector<uint8_t>& body)
{
    DANEJOE_LOG_TRACE("default", "ClientMessageCodec", "Parse download response");
    DownloadResponseTransfer info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(body);

    auto file_id_field_opt = serializer.get_parsed_field("file_id");
    auto file_name_field_opt = serializer.get_parsed_field("file_name");
    auto source_path_field_opt = serializer.get_parsed_field("source_path");
    auto file_size_field_opt = serializer.get_parsed_field("file_size");
    auto md5_code_field_opt = serializer.get_parsed_field("md5_code");

    if (file_id_field_opt.has_value())
    {
        auto file_id_op = DaneJoe::to_value<int64_t>(file_id_field_opt.value());
        if (file_id_op.has_value())
        {
            info.file_id = file_id_op.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "File id parse failed");
            return std::nullopt;
        }
    }

    if (file_name_field_opt.has_value())
    {
        info.saved_name = DaneJoe::to_string(file_name_field_opt.value());
    }

    if (source_path_field_opt.has_value())
    {
        info.source_path = DaneJoe::to_string(source_path_field_opt.value());
    }

    if (file_size_field_opt.has_value())
    {
        auto file_size_op = DaneJoe::to_value<int64_t>(file_size_field_opt.value());
        if (file_size_op.has_value())
        {
            info.file_size = file_size_op.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "File size parse failed");
            return std::nullopt;
        }
    }

    if (md5_code_field_opt.has_value())
    {
        info.md5_code = DaneJoe::to_string(md5_code_field_opt.value());
    }

    return info;
}

std::optional<BlockResponseTransfer> ClientMessageCodec::try_parse_block_response(const std::vector<uint8_t>& body)
{
    BlockResponseTransfer info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(body);

    auto block_id_field_op = serializer.get_parsed_field("block_id");
    auto file_id_field_op = serializer.get_parsed_field("file_id");
    auto offset_field_op = serializer.get_parsed_field("offset");
    auto block_size_field_op = serializer.get_parsed_field("block_size");
    auto data_field_op = serializer.get_parsed_field("data");

    if (!block_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Block id parse failed");
        return std::nullopt;
    }
    auto block_id_op = DaneJoe::to_value<int64_t>(block_id_field_op.value());
    if (!block_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Block id parse failed");
        return std::nullopt;
    }
    info.block_id = block_id_op.value();

    if (!file_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    auto file_id_op = DaneJoe::to_value<int64_t>(file_id_field_op.value());
    if (!file_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    info.file_id = file_id_op.value();

    if (!offset_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Offset parse failed");
        return std::nullopt;
    }
    auto offset_op = DaneJoe::to_value<int64_t>(offset_field_op.value());
    if (!offset_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Offset parse failed");
        return std::nullopt;
    }
    info.offset = offset_op.value();

    if (!block_size_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Block size parse failed");
        return std::nullopt;
    }
    auto block_size_op = DaneJoe::to_value<int64_t>(block_size_field_op.value());
    if (!block_size_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Block size parse failed");
        return std::nullopt;
    }
    info.block_size = block_size_op.value();

    if (!data_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ClientMessageCodec", "Data parse failed");
        return std::nullopt;
    }
    info.data = DaneJoe::to_array<uint8_t>(data_field_op.value());

    return info;
}

std::optional<TestResponseTransfer> ClientMessageCodec::try_parse_test_response(const std::vector<uint8_t>& body)
{
    DaneJoe::SerializeCodec serializer;
    auto header_opt = serializer.get_message_header(body);
    DANEJOE_LOG_DEBUG("default", "ClientMessageCodec", "Header: {}", header_opt.has_value() ? header_opt->to_string() : "Invalid header");
    serializer.deserialize(body);

    auto message_field_op = serializer.get_parsed_field("message");
    if (!message_field_op.has_value())
    {
        DANEJOE_LOG_WARN("default", "ClientMessageCodec", "Failed to handle Test response: message field not found");
        return std::nullopt;
    }

    TestResponseTransfer info;
    info.message = DaneJoe::to_string(message_field_op.value());
    return info;
}

std::vector<uint8_t> ClientMessageCodec::build_request(EnvelopeRequestTransfer info)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(info.version, "version");
    serializer.serialize(info.request_id, "request_id");
    serializer.serialize(info.request_type, "request_type");
    serializer.serialize(info.path, "path");
    serializer.serialize(static_cast<uint8_t>(info.content_type), "content_type");
    serializer.serialize(info.body, "body");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> ClientMessageCodec::build_test_request(const TestRequestTransfer& test_request, int64_t request_id)
{
    // 构建消息体
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(test_request.message, "message");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    // 构建Envelope请求
    EnvelopeRequestTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.request_type = 1; // POST
    envelope.path = "/test";
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;

    return build_request(envelope);
}

std::vector<uint8_t> ClientMessageCodec::build_download_request(const DownloadRequestTransfer& download_request, int64_t request_id)
{
    DANEJOE_LOG_TRACE("default", "ClientMessageCodec", "Building download request for file_id: {}", download_request.file_id);
    // 构建消息体
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(download_request.file_id, "file_id");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    // 构建Envelope请求
    EnvelopeRequestTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.request_type = 0; // GET
    envelope.path = "/download";
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;

    return build_request(envelope);
}

std::vector<uint8_t> ClientMessageCodec::build_block_request(const BlockRequestTransfer& block_request, int64_t request_id)
{
    DANEJOE_LOG_TRACE("default", "ClientMessageCodec", "Building block request for block: {}", block_request.to_string());
    // 构建消息体
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(block_request.block_id, "block_id");
    body_serializer.serialize(block_request.file_id, "file_id");
    body_serializer.serialize(block_request.offset, "offset");
    body_serializer.serialize(block_request.block_size, "block_size");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    // 构建Envelope请求
    EnvelopeRequestTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.request_type = 0; // GET
    envelope.path = "/block";
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;

    return build_request(envelope);
}

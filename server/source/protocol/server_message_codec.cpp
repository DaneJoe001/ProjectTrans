#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/network/codec/serialize_codec.hpp"

#include "protocol/server_message_codec.hpp"

std::optional<EnvelopeRequestTransfer> ServerMessageCodec::try_parse_byte_array_request(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ServerMessageCodec", "Parsing envelope request");
    EnvelopeRequestTransfer envelope;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(data);

    auto version_field_opt = serializer.get_parsed_field("version");
    auto request_id_field_opt = serializer.get_parsed_field("request_id");
    auto request_type_field_opt = serializer.get_parsed_field("request_type");
    auto path_field_opt = serializer.get_parsed_field("path");
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
            DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Version parse failed");
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
            DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Request id parse failed");
            return std::nullopt;
        }
    }

    if (request_type_field_opt.has_value())
    {
        auto request_type_op = DaneJoe::to_value<uint8_t>(request_type_field_opt.value());
        if (request_type_op.has_value())
        {
            envelope.request_type = request_type_op.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "request_type parse failed");
            return std::nullopt;
        }
    }

    if (path_field_opt.has_value())
    {
        auto path_op = DaneJoe::to_string(path_field_opt.value());
        if (!path_op.empty())
        {
            envelope.path = path_op;
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Path parse failed");
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
            DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Content type parse failed");
            return std::nullopt;
        }
    }

    if (body_field_opt.has_value())
    {
        envelope.body = DaneJoe::to_array<uint8_t>(body_field_opt.value());
    }

    return envelope;
}

std::optional<BlockRequestTransfer> ServerMessageCodec::try_parse_byte_array_block_request(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ServerMessageCodec", "Parse block request");
    BlockRequestTransfer info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(data);

    auto block_id_field_op = serializer.get_parsed_field("block_id");
    auto file_id_field_op = serializer.get_parsed_field("file_id");
    auto task_id_field_op = serializer.get_parsed_field("task_id");
    auto offset_field_op = serializer.get_parsed_field("offset");
    auto block_size_field_op = serializer.get_parsed_field("block_size");

    if (!block_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Block id parse failed");
        return std::nullopt;
    }
    auto block_id_op = DaneJoe::to_value<int64_t>(block_id_field_op.value());
    if (!block_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Block id parse failed");
        return std::nullopt;
    }
    info.block_id = block_id_op.value();

    if (!file_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    auto file_id_op = DaneJoe::to_value<int64_t>(file_id_field_op.value());
    if (!file_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    info.file_id = file_id_op.value();

    if (task_id_field_op.has_value())
    {
        auto task_id_op = DaneJoe::to_value<int64_t>(task_id_field_op.value());
        if (task_id_op.has_value())
        {
            info.task_id = task_id_op.value();
        }
    }

    if (!offset_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Offset parse failed");
        return std::nullopt;
    }
    auto offset_op = DaneJoe::to_value<int64_t>(offset_field_op.value());
    if (!offset_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Offset parse failed");
        return std::nullopt;
    }
    info.offset = offset_op.value();

    if (!block_size_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Block size parse failed");
        return std::nullopt;
    }
    auto block_size_op = DaneJoe::to_value<int64_t>(block_size_field_op.value());
    if (!block_size_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "Block size parse failed");
        return std::nullopt;
    }
    info.block_size = block_size_op.value();

    return info;
}

std::optional<DownloadRequestTransfer> ServerMessageCodec::try_parse_byte_array_download_request(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ServerMessageCodec", "Parse download request");
    DownloadRequestTransfer info;
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(data);

    auto file_id_field_opt = serializer.get_parsed_field("file_id");
    auto task_id_field_opt = serializer.get_parsed_field("task_id");

    if (!file_id_field_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    auto file_id_op = DaneJoe::to_value<int64_t>(file_id_field_opt.value());
    if (!file_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "ServerMessageCodec", "File id parse failed");
        return std::nullopt;
    }
    info.file_id = file_id_op.value();

    if (task_id_field_opt.has_value())
    {
        auto task_id_op = DaneJoe::to_value<int64_t>(task_id_field_opt.value());
        if (task_id_op.has_value())
        {
            info.task_id = task_id_op.value();
        }
        else
        {
            DANEJOE_LOG_WARN("default", "ServerMessageCodec", "Task id parse failed");
        }
    }

    return info;
}

std::optional<TestRequestTransfer> ServerMessageCodec::try_parse_byte_array_test_request(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ServerMessageCodec", "Parse test request");
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(data);

    auto message_field_op = serializer.get_parsed_field("message");
    if (!message_field_op.has_value())
    {
        DANEJOE_LOG_WARN("default", "ServerMessageCodec", "Failed to handle Test request: message field not found");
        return std::nullopt;
    }

    TestRequestTransfer info;
    info.message = DaneJoe::to_string(message_field_op.value());
    return info;
}

std::vector<uint8_t> ServerMessageCodec::build_response_byte_array(const EnvelopeResponseTransfer& response)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(response.version, "version");
    serializer.serialize(response.request_id, "request_id");
    serializer.serialize(static_cast<uint16_t>(response.status), "status");
    serializer.serialize(static_cast<uint8_t>(response.content_type), "content_type");
    serializer.serialize(response.body, "body");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> ServerMessageCodec::build_block_response_byte_array(const BlockResponseTransfer& block_response, int64_t request_id)
{
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(block_response.block_id, "block_id");
    body_serializer.serialize(block_response.file_id, "file_id");
    body_serializer.serialize(block_response.task_id, "task_id");
    body_serializer.serialize(block_response.offset, "offset");
    body_serializer.serialize(block_response.block_size, "block_size");
    body_serializer.serialize(block_response.data, "data");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    EnvelopeResponseTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.status = ResponseStatus::Ok;
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;
    return build_response_byte_array(envelope);
}

std::vector<uint8_t> ServerMessageCodec::build_download_response_byte_array(const DownloadResponseTransfer& download_response, int64_t request_id)
{
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(download_response.task_id, "task_id");
    body_serializer.serialize(download_response.file_id, "file_id");
    body_serializer.serialize(download_response.file_name, "file_name");
    body_serializer.serialize(download_response.file_size, "file_size");
    body_serializer.serialize(download_response.md5_code, "md5_code");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    EnvelopeResponseTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.status = ResponseStatus::Ok;
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;
    return build_response_byte_array(envelope);
}

std::vector<uint8_t> ServerMessageCodec::build_test_response_byte_array(const TestResponseTransfer& test_response, int64_t request_id)
{
    DaneJoe::SerializeCodec body_serializer;
    body_serializer.serialize(test_response.message, "message");
    std::vector<uint8_t> body = body_serializer.get_serialized_data_vector_build();

    EnvelopeResponseTransfer envelope;
    envelope.version = 1;
    envelope.request_id = request_id;
    envelope.status = ResponseStatus::Ok;
    envelope.content_type = ContentType::DaneJoe;
    envelope.body = body;
    return build_response_byte_array(envelope);
}
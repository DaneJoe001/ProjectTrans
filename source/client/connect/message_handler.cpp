#include <cstdio>
#include <chrono>

#include "client/connect/message_handler.hpp"
#include "log/manage_logger.hpp"
#include "common/network/danejoe_serializer.hpp"

UrlResolver::UrlInfo MessageHandler::add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password)
{
    // 拷贝原始信息
    UrlResolver::UrlInfo result_info = raw_info;
    // 检查协议
    if (raw_info.protocol == UrlResolver::UrlProtocol::DANEJOE)
    {
        // 添加账户信息
        // @todo 加密
        result_info.path += "?account=" + account + "&password=" + password;
    }
    return result_info;
}

ClientFileInfo MessageHandler::parse_raw_file_info(const std::vector<uint8_t>& raw_data, Operation operation)
{
    /// @todo 添加文件名长度限制/移除硬编码数值/添加长度信息(序列化添加)
    DANEJOE_LOG_TRACE("default", "MessageHandler", "Parse file info");
    ClientFileInfo result_info;
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(raw_data);
    auto file_id_field = serializer.get_parsed_field("file_id");
    auto file_name_field = serializer.get_parsed_field("file_name");
    auto file_size_field = serializer.get_parsed_field("file_size");
    auto md5_code_field = serializer.get_parsed_field("md5_code");
    if (file_id_field.has_value())
    {
        auto file_id_it = DaneJoe::to_value<int32_t>(file_id_field.value());
        if (file_id_it.has_value())
        {
            result_info.file_id = file_id_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
            return ClientFileInfo{};
        }
    }
    if (file_name_field.has_value())
    {
        // 默认的保存名
        result_info.saved_name = DaneJoe::to_string(file_name_field.value());
    }
    if (file_size_field.has_value())
    {
        auto file_size_it = DaneJoe::to_value<uint32_t>(file_size_field.value());
        if (file_size_it.has_value())
        {
            result_info.file_size = file_size_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "File size parse failed");
            return ClientFileInfo{};
        }
    }
    if (md5_code_field.has_value())
    {
        result_info.md5_code = DaneJoe::to_string(md5_code_field.value());
    }
    // 初始化文件状态
    result_info.state = FileState::Waiting;
    result_info.create_time = std::chrono::system_clock::now();
    return result_info;
}

std::list<BlockRequestInfo> MessageHandler::calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config)
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

std::vector<uint8_t> MessageHandler::build_block_request(const BlockRequestInfo& block_request_info)
{
    DaneJoe::DaneJoeSerializer serializer;
    serializer.serialize(block_request_info.block_id, "block_id").serialize(block_request_info.file_id, "file_id").serialize(block_request_info.offset, "offset").serialize(block_request_info.block_size, "block_size");
    return serializer.get_serialized_data_vector_build();
}

void MessageHandler::parse_block_response(const std::vector<uint8_t>& raw_data)
{
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(raw_data);
    auto block_id_field_op = serializer.get_parsed_field("block_id");
    auto file_id_field_op = serializer.get_parsed_field("file_id");
    auto offset_field_op = serializer.get_parsed_field("offset");
    auto block_size_field_op = serializer.get_parsed_field("block_size");
    auto data_field_op = serializer.get_parsed_field("data");
    if (!block_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block id parse failed");
        return;
    }
    auto block_id_op = DaneJoe::to_value<int32_t>(block_id_field_op.value());
    if (!block_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block id parse failed");
        return;
    }
    uint32_t block_id = block_id_op.value();
    if (!file_id_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
        return;
    }
    auto file_id_op = DaneJoe::to_value<int32_t>(file_id_field_op.value());
    if (!file_id_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "File id parse failed");
        return;
    }
    uint32_t file_id = file_id_op.value();
    if (!offset_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Offset parse failed");
        return;
    }
    auto offset_op = DaneJoe::to_value<uint32_t>(offset_field_op.value());
    if (!offset_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Offset parse failed");
        return;
    }
    uint32_t offset = offset_op.value();
    if (!block_size_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block size parse failed");
        return;
    }
    auto block_size_op = DaneJoe::to_value<uint32_t>(block_size_field_op.value());
    if (!block_size_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Block size parse failed");
        return;
    }
    uint32_t block_size = block_size_op.value();
    if (!data_field_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "Data parse failed");
        return;
    }
    std::vector<uint8_t> data = DaneJoe::to_array<uint8_t>(data_field_op.value());
    DANEJOE_LOG_TRACE("default", "MessageHandler", "Block id: {}, file id: {}, offset: {}, block size: {}, data size: {}", block_id, file_id, offset, block_size, data.size());
}
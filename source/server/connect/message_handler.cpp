#include <fstream>

#include "server/connect/message_handler.hpp"
#include "log/manage_logger.hpp"
#include "common/network/danejoe_serializer.hpp"

ServerFileInfoService MessageHandler::m_file_info_service;

std::vector<uint8_t> MessageHandler::build_download_response(const ServerFileInfo& file_info)
{
    DaneJoe::DaneJoeSerializer serializer;
    serializer.serialize(file_info.file_id, "file_id").
        serialize(file_info.file_name, "file_name").
        serialize(file_info.file_size, "file_size").
        serialize(file_info.md5_code, "md5_code");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> MessageHandler::build_upload_response(const ServerFileInfo& file_info)
{
    DaneJoe::DaneJoeSerializer serializer;
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> MessageHandler::build_test_response(const ServerFileInfo& file_info)
{
    DaneJoe::DaneJoeSerializer serializer;
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> MessageHandler::build_block_response(const BlockResponseInfo& block_response_info)
{
    DaneJoe::DaneJoeSerializer serializer;
    serializer.serialize(block_response_info.block_id, "block_id").serialize(block_response_info.data, "data").
        serialize(block_response_info.file_id, "file_id").
        serialize(block_response_info.block_size, "block_size").
        serialize(block_response_info.offset, "offset");
    return serializer.get_serialized_data_vector_build();
}

BlockResponseInfo MessageHandler::build_block_data(const std::string& data)
{
    std::vector<uint8_t> bytes = std::vector<uint8_t>(data.begin(), data.end());
    return build_block_data(bytes);
}

BlockResponseInfo MessageHandler::build_block_data(const std::vector<uint8_t>& data)
{
    BlockResponseInfo block_response_info;
    DaneJoe::DaneJoeSerializer serializer;
    serializer.deserialize(data);
    auto file_id_field = serializer.get_parsed_field("file_id");
    auto block_id_field = serializer.get_parsed_field("block_id");
    auto offset_field = serializer.get_parsed_field("offset");
    auto block_size_field = serializer.get_parsed_field("block_size");
    if (file_id_field)
    {
        auto file_id_it = to_value<uint32_t>(file_id_field.value());
        if (file_id_it)
        {
            block_response_info.file_id = file_id_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "file_id is not found");
            return BlockResponseInfo();
        }
    }
    if (block_id_field)
    {
        auto block_id_it = to_value<uint32_t>(block_id_field.value());
        if (block_id_it)
        {
            block_response_info.block_id = block_id_it.value();
        }
        else
        {
            // block_id不影响实际传输
            DANEJOE_LOG_WARN("default", "MessageHandler", "block_id is not found");
        }
    }
    if (offset_field)
    {
        auto offset_it = to_value<uint32_t>(offset_field.value());
        if (offset_it)
        {
            block_response_info.offset = offset_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "offset is not found");
            return BlockResponseInfo();
        }
    }
    if (block_size_field)
    {
        auto block_size_it = to_value<uint32_t>(block_size_field.value());
        if (block_size_it)
        {
            block_response_info.block_size = block_size_it.value();
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MessageHandler", "block_size is not found");
            return BlockResponseInfo();
        }
    }
    m_file_info_service.init();
    auto server_file_info_op = m_file_info_service.get_by_id(block_response_info.file_id);
    if (!server_file_info_op.has_value())
    {
        DANEJOE_LOG_ERROR("default", "MessageHandler", "file_id is not found");
        return BlockResponseInfo();
    }
    auto server_file_info = server_file_info_op.value();
    block_response_info.data.resize(block_response_info.block_size);
    std::ifstream fin(server_file_info.resource_path, std::ios::binary);
    fin.seekg(block_response_info.offset);
    fin.readsome(reinterpret_cast<char*>(block_response_info.data.data()), block_response_info.block_size);
    return block_response_info;
}
#include <cstdio>
#include <chrono>

#include "client/connect/message_handler.hpp"
#include "log/manage_logger.hpp"

UrlResolver::UrlInfo MessageHandler::add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password)
{
    UrlResolver::UrlInfo result_info = raw_info;
    if (raw_info.protocol == UrlResolver::UrlProtocol::DANEJOE)
    {
        result_info.path += "?account=" + account + "&password=" + password;
    }
    return result_info;
}

ClientFileInfo MessageHandler::parse_raw_file_info(const std::string& raw_data, Operation operation)
{
    DANEJOE_LOG_TRACE("default", "MessageHandler", "Raw data: {}", raw_data);
    /// @todo 添加自定义协议标记
    /// @todo 当前简化解析逻辑，未采用序列化，需要进行格式验证
    ClientFileInfo result_info;
    /// @todo 添加文件名长度限制/移除硬编码数值/添加长度信息(序列化添加)
    /// @todo 考虑是否添加raw_file_name字段
    std::string file_name;
    file_name.resize(256);
    result_info.source_path.resize(256);
    result_info.md5_code.resize(128);
    result_info.operation = operation;

    int matched = std::sscanf(raw_data.c_str(),
        "file_id: %d, file_name: %255[^,], resource_path: %255[^,], file_size: %u, md5_code: %127s",
        &result_info.file_id,
        file_name.data(),
        result_info.source_path.data(),
        &result_info.file_size,
        result_info.md5_code.data());

    result_info.state = FileState::Waiting;
    result_info.create_time = std::chrono::system_clock::now();
    return result_info;
}

std::list<BlockRequestInfo> MessageHandler::calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config)
{
    /// @todo 根据硬件以及手动设置，来完成更具体地请求分块
    std::list<BlockRequestInfo> result_info;
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

std::string build_block_request(const BlockRequestInfo& block_request_info)
{
    std::string request = "/block?value=" + block_request_info.to_string();
    return request;
}
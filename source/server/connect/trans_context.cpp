#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <queue>

#include "server/connect/trans_context.hpp"
#include "log/manage_logger.hpp"
#include "server/connect/server_trans.hpp"
#include "server/model/server_file_info.hpp"

/// @todo 修复添加长度后的接收问题
// #define ADD_LENGTH_INFO_TO_RECV
// #define ADD_LENGTH_INFO_TO_SEND

void TransContext::on_recv()
{
#ifdef ADD_LENGTH_INFO_TO_RECV
    // 数据长度
    uint32_t size = 0;
    if (!is_got_length_info)
    {// 长度不足直接返回
        if (m_recv_buffer->size() < sizeof(uint32_t))
        {
            return;
        }
        // 存储长度数据
        std::vector<uint8_t> length_data;
#define iS_NON_BLOCKING_POP
#ifdef iS_NON_BLOCKING_POP
        length_data = m_recv_buffer->try_pop(sizeof(uint32_t));
#else 
        auto length_data_optional = m_recv_buffer->pop(sizeof(uint32_t));
        // 判断是否存在有效数据
        if (!length_data_optional.has_value())
        {
            return;
        }
        else
        {
            length_data = length_data_optional.value();
        }
#endif
        if (length_data.empty() || length_data.size() != sizeof(uint32_t))
        {
            return;
        }
        std::memcpy(&size, length_data.data(), sizeof(uint32_t));
        if (size == 0)
        {
            return;
        }
        is_got_length_info = true;
    }
    if (m_recv_buffer->size() < size)
    {
        return;
    }
    // 存储接收的数据
    std::vector<uint8_t> data;
#ifdef iS_NON_BLOCKING_POP
    data = m_recv_buffer->try_pop(size);
#else
    auto data_optional = m_recv_buffer->pop(size);

    if (data_optional.has_value())
    {
        data = data_optional.value();
    }
    // 更新用于下一次接收数据
    is_got_length_info = false;
#endif
#else
    auto data = m_recv_buffer->try_pop(1024);
#endif

    std::string str(data.begin(), data.end());
    if (!str.empty())
    {
        DANEJOE_LOG_INFO("default", "TransContext", "TransContext received: {}", str);
    }
    for (auto ch : str)
    {
        m_queue.push((uint8_t)ch);
    }
}

void TransContext::on_send()
{
#ifdef ADD_LENGTH_INFO_TO_SEND
    // 获取当前队列的长度
    uint32_t size = m_queue.size();
    if (size == 0)
    {
        return;
    }
    // 构建要发送的数据长度信息
    std::vector<uint8_t> length_data(sizeof(uint32_t));
    std::memcpy(length_data.data(), &size, sizeof(uint32_t));
    DANEJOE_LOG_TRACE("default", "TransContext", "TransContext send length: {}", size);
    // 发送数据长度信息
    m_send_buffer->push(length_data.begin(), length_data.end());
    // 弹出当前确认的所有数据
#define iS_NON_BLOCKING_POP
    std::vector<uint8_t> data = m_queue.try_pop(size);
#ifdef iS_NON_BLOCKING_POP
#else
    auto data_optional = m_queue.pop(size);
    std::vector<uint8_t> data;
    if (!data_optional.has_value())
    {
        DANEJOE_LOG_TRACE("default", "TransContext", "Failed to pop data");
        return;
    }
    data = data_optional.value();
#endif

#else
    auto data = m_queue.try_pop(1024);
#endif
    if (data.empty())
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "TransContext", "TransContext send: {}", std::string(data.begin(), data.end()));
    // 解析请求链接
    auto request_info = parse_request(data);
    // 检查是否解析出键值对
    if (request_info.info.size() == 0)
    {
        std::string invalid_info = "Invalid request";
        std::vector<uint8_t> invalid_info_vector(invalid_info.begin(), invalid_info.end());
        m_send_buffer->push(invalid_info_vector.begin(), invalid_info_vector.end());
        return;
    }
    switch (request_info.type)
    {
    case RequestType::Download:
        handle_download_request(request_info);
        break;
    case RequestType::Upload:
        handle_upload_request(request_info);
        break;
    case RequestType::Test:
        handle_test_request(request_info);
        break;
    case RequestType::Block:
        handle_block_request(request_info);
        break;
    default:
        break;
    }
}

void TransContext::handle_download_request(const RequestInfo& request_info)
{
    if (request_info.type != RequestType::Download)
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: request type is not download");
        return;
    }
    ServerFileInfo file_info;
    // 通过id获取文件信息
    if (request_info.info.count("file_id") != 0)
    {
        int32_t file_id = std::stoi(request_info.info.at("file_id"));
        auto file_info_optional = m_file_info_service.get_by_id(file_id);
        if (!file_info_optional.has_value())
        {
            DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: file info not found");
            return;
        }
        file_info = file_info_optional.value();
    }
    if (file_info.file_id == -1)
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: file info not found");
        return;
    }
    std::string file_info_str = file_info.to_string();
    std::vector<uint8_t> file_info_vector(file_info_str.begin(), file_info_str.end());
    m_send_buffer->push(file_info_vector.begin(), file_info_vector.end());
}

void TransContext::handle_upload_request(const RequestInfo& request_info)
{
    if (request_info.type != RequestType::Upload)
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: request type is not download");
        return;
    }
    std::string response = "Recieved upload request";
    std::vector<uint8_t> response_vector = std::vector<uint8_t>(response.begin(), response.end());
    m_send_buffer->push(response_vector.begin(), response_vector.end());
}

void TransContext::handle_test_request(const RequestInfo& request_info)
{
    if (request_info.type != RequestType::Test)
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: request type is not download");
        return;
    }
    auto content_it = request_info.info.find("content");
    std::string content;
    if (content_it != request_info.info.end())
    {
        content = content_it->second;
    }
    std::string response = "Recieved test content: " + content;
    std::vector<uint8_t> response_vector = std::vector<uint8_t>(response.begin(), response.end());
    m_send_buffer->push(response_vector.begin(), response_vector.end());
}

void TransContext::handle_block_request(const RequestInfo& request_info)
{
    if (request_info.type != RequestType::Test)
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: request type is not download");
        return;
    }
    auto value_it = request_info.info.find("value");
    if (value_it == request_info.info.end())
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "TransContext", "Received block request: {}", value_it->second);
}

std::shared_ptr<ISocketContext> TransContextCreator::create()
{
    return std::make_shared<TransContext>();
}

std::shared_ptr<ISocketContext> TransContextCreator::create(
    std::shared_ptr<DaneJoe::MTQueue<uint8_t>> recv_buffer,
    std::shared_ptr<DaneJoe::MTQueue<uint8_t>> send_buffer)
{
    auto context = std::make_shared<TransContext>();
    context->set_recv_buffer(recv_buffer);
    context->set_send_buffer(send_buffer);
    ServerTrans::get_instance().register_trans(context);
    return context;
}

void TransContextCreator::destroy(std::shared_ptr<ISocketContext> context)
{
    auto trans_context = std::dynamic_pointer_cast<TransContext>(context);
    ServerTrans::get_instance().unregister_trans(trans_context);
}

TransContext::RequestInfo TransContext::parse_request(const std::vector<uint8_t>& data)
{
    RequestInfo result;
    /// @todo 完善多个键值对的处理
    //example: /download?file=file.txt
    std::string url(data.begin(), data.end());
    DANEJOE_LOG_TRACE("default", "TransContext", "Parse_request received: {}", url);
    // 解析下载请求
    auto download_pos = url.find("download");
    if (download_pos != std::string::npos)
    {
        result.type = RequestType::Download;
        auto question_pos = url.find("?");
        auto equal_pos = url.find("=");
        if (question_pos == std::string::npos || equal_pos == std::string::npos)
        {
            DANEJOE_LOG_ERROR("default", "TransContext", "Parse_request failed: {}", url);
            return result;
        }
        auto end_pos = url.find("&");
        if (end_pos == std::string::npos)
        {
            end_pos = url.size();
        }
        uint32_t key_length = equal_pos - question_pos - 1;
        std::string key = url.substr(question_pos + 1, key_length);
        std::string value = url.substr(equal_pos + 1, end_pos - equal_pos - 1);
        result.info[key] = value;
        return result;
    }
    auto test_pos = url.find("test");
    if (test_pos != std::string::npos)
    {
        //example: /test?content=file.txt
        result.type = RequestType::Test;
        auto question_pos = url.find("?");
        auto equal_pos = url.find("=");
        if (question_pos == std::string::npos || equal_pos == std::string::npos)
        {
            DANEJOE_LOG_ERROR("default", "TransContext", "Parse_request failed: {}", url);
            return result;
        }
        auto end_pos = url.find("&");
        if (end_pos == std::string::npos)
        {
            end_pos = url.size();
        }
        uint32_t key_length = equal_pos - question_pos - 1;
        std::string key = url.substr(question_pos + 1, key_length);
        std::string value = url.substr(equal_pos + 1, end_pos - equal_pos - 1);
        result.info[key] = value;
        return result;
    }
    auto block_pos = url.find("block");
    if (block_pos != std::string::npos)
    {
        result.type = RequestType::Block;
        auto question_pos = url.find("?");
        auto equal_pos = url.find("=");
        if (question_pos == std::string::npos || equal_pos == std::string::npos)
        {
            DANEJOE_LOG_ERROR("default", "TransContext", "Parse_request failed: {}", url);
            return result;
        }
        auto end_pos = url.find("&");
        if (end_pos == std::string::npos)
        {
            end_pos = url.size();
        }
        uint32_t key_length = equal_pos - question_pos - 1;
        std::string key = url.substr(question_pos + 1, key_length);
        std::string value = url.substr(equal_pos + 1, end_pos - equal_pos - 1);
        result.info[key] = value;
        return result;
    }
    return result;
}

TransContext::TransContext()
{
    m_file_info_service.init();
}

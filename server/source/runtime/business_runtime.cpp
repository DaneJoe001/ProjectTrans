#include <fstream>

#include "danejoe/logger/logger_manager.hpp"
#include "runtime/business_runtime.hpp"

BusinessRuntime::BusinessRuntime(std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box) :
    m_reactor_mail_box(reactor_mail_box)
{
}

void BusinessRuntime::init()
{
    m_file_info_service.init();
}
void BusinessRuntime::run()
{
    DANEJOE_LOG_INFO("default", "BusinessRuntime", "Business runtime thread started");
    m_is_running.store(true);
    while (m_is_running)
    {
        auto frame_opt = m_reactor_mail_box->pop_from_to_server_frame();
        if (!frame_opt.has_value())
        {
            continue;
        }
        DANEJOE_LOG_DEBUG("default", "BusinessRuntime", "Received frame: connect_id={}, size={}",
            frame_opt.value().connect_id,
            frame_opt.value().data.size());
        handle_request(frame_opt.value().data, frame_opt.value().connect_id);
    }
    DANEJOE_LOG_WARN("default", "BusinessRuntime", "Business runtime thread exited");
}
void BusinessRuntime::stop()
{
    m_is_running.store(false);
}

void BusinessRuntime::handle_request(
    const std::vector<uint8_t>& frame_data,
    uint64_t connect_id)
{
    auto request_opt = m_message_codec.try_parse_byte_array_request(frame_data);
    if (!request_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "BusinessRuntime", "Parse request failed: connect_id={}, frame_size={}", connect_id, frame_data.size());
        return;
    }
    EnvelopeRequestTransfer request_transfer = request_opt.value();
    DANEJOE_LOG_DEBUG("default", "BusinessRuntime", "Received request: connect_id={}, {}", connect_id, request_transfer.to_string());
    if (request_transfer.path == "/download")
    {
        auto download_request_opt = m_message_codec.try_parse_byte_array_download_request(request_transfer.body);
        if (!download_request_opt.has_value())
        {
            return;
        }
        handle_download_request(download_request_opt.value(), request_transfer.request_id, connect_id);
    }
    else if (request_transfer.path == "/test")
    {
        auto test_request_opt = m_message_codec.try_parse_byte_array_test_request(request_transfer.body);
        if (!test_request_opt.has_value())
        {
            return;
        }
        handle_test_request(test_request_opt.value(), request_transfer.request_id, connect_id);
    }
    else if (request_transfer.path == "/block")
    {
        auto block_request_opt = m_message_codec.try_parse_byte_array_block_request(request_transfer.body);
        if (!block_request_opt.has_value())
        {
            return;
        }
        handle_block_request(block_request_opt.value(), request_transfer.request_id, connect_id);
    }
    else
    {
        handle_unknown_request();
    }
}

void BusinessRuntime::handle_unknown_request()
{}
void BusinessRuntime::handle_download_request(
    const DownloadRequestTransfer& download_request,
    int64_t request_id,
    uint64_t connect_id)
{
    auto file_entity_opt = m_file_info_service.get_by_id(download_request.file_id);
    if (!file_entity_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "BusinessRuntime", "Download request file not found: connect_id={}, request_id={}, file_id={}",
            connect_id,
            request_id,
            download_request.file_id);
        DownloadResponseTransfer response;
        response.file_id = download_request.file_id;
        response.task_id = download_request.task_id;
        response.file_name = "";
        response.file_size = 0;
        response.md5_code = "";
        auto data = m_message_codec.build_download_response_byte_array(response, request_id);
        if (m_reactor_mail_box)
        {
            m_reactor_mail_box->push_to_client_frame({ connect_id, data });
        }
        return;
    }
    ServerFileInfo file_entity = file_entity_opt.value();
    DANEJOE_LOG_TRACE("default", "TransContext", "Handling download request for file: {}", file_entity.to_string());
    DownloadResponseTransfer response;
    response.file_id = file_entity.file_id;
    response.task_id = download_request.task_id;
    response.file_name = file_entity.file_name;
    response.file_size = file_entity.file_size;
    response.md5_code = file_entity.md5_code;
    auto data = m_message_codec.build_download_response_byte_array(response, request_id);
    if (m_reactor_mail_box)
    {
        m_reactor_mail_box->push_to_client_frame({ connect_id,data });
    }
}

void BusinessRuntime::handle_test_request(
    const TestRequestTransfer& test_request,
    int64_t request_id,
    uint64_t connect_id)
{
    // 解析测试请求消息体
    auto message = test_request.message;
    DANEJOE_LOG_TRACE("default", "TransContext", "Received test request: {}", message);
    TestResponseTransfer response;
    response.message = "Echo: " + message;
    // 构建测试响应,当前仅做回显
    std::vector<uint8_t> data = m_message_codec.build_test_response_byte_array(response, request_id);
    if (m_reactor_mail_box)
    {
        m_reactor_mail_box->push_to_client_frame({ connect_id,data });
    }
}

void BusinessRuntime::handle_block_request(
    const BlockRequestTransfer& block_request,
    int64_t request_id,
    uint64_t connect_id)
{
    auto file_entity = m_file_info_service.get_by_id(block_request.file_id);
    if (!file_entity.has_value())
    {
        DANEJOE_LOG_WARN("default", "BusinessRuntime", "Block request file not found: connect_id={}, request_id={}, file_id={}, block_id={}",
            connect_id,
            request_id,
            block_request.file_id,
            block_request.block_id);
        BlockResponseTransfer response;
        response.block_id = block_request.block_id;
        response.file_id = block_request.file_id;
        response.task_id = block_request.task_id;
        response.offset = block_request.offset;
        response.block_size = 0;
        response.data = {};
        auto data = m_message_codec.build_block_response_byte_array(response, request_id);
        if (m_reactor_mail_box)
        {
            m_reactor_mail_box->push_to_client_frame({ connect_id, data });
        }
        return;
    }
    BlockResponseTransfer response;
    response.block_id = block_request.block_id;
    response.file_id = block_request.file_id;
    response.task_id = block_request.task_id;
    response.offset = block_request.offset;
    response.block_size = block_request.block_size;
    response.data = std::vector<uint8_t>(block_request.block_size);

    std::ifstream fin(file_entity->resource_path, std::ios::in | std::ios::binary);
    if (!fin.is_open())
    {
        DANEJOE_LOG_WARN("default", "BusinessRuntime", "Block request open file failed: connect_id={}, request_id={}, file_id={}, path={}",
            connect_id,
            request_id,
            block_request.file_id,
            file_entity->resource_path);
        response.block_size = 0;
        response.data = {};
        auto data = m_message_codec.build_block_response_byte_array(response, request_id);
        if (m_reactor_mail_box)
        {
            m_reactor_mail_box->push_to_client_frame({ connect_id, data });
        }
        return;
    }
    fin.seekg(block_request.offset);
    fin.read(reinterpret_cast<char*>(response.data.data()), block_request.block_size);

    auto data = m_message_codec.build_block_response_byte_array(response, request_id);
    // 将块响应写入发送缓冲区
    if (m_reactor_mail_box)
    {
        m_reactor_mail_box->push_to_client_frame({ connect_id,data });
    }
}
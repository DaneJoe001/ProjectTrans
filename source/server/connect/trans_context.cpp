#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <queue>

#include "common/log/manage_logger.hpp"
#include "common/network/danejoe_serializer.hpp"
#include "common/protocol/danejoe_protocol.hpp"
#include "server/connect/trans_context.hpp"
#include "server/connect/server_trans.hpp"
#include "server/model/server_file_info.hpp"
#include "server/connect/message_handler.hpp"
#include "server/model/block_response_info.hpp"

    /// @todo 客户端处理url，统一序列化，不使用字符串路径传递参数
    /// @todo 如何处理客户端,区分http协议和自定义协议
    /// @todo 请求=[类型+消息体]
    /// @todo 响应=协议信息+[消息体]
        /// 现在考虑的问题是数据长度放在哪一部分？
    /// 当前具有的实现是，序列化中含有数据包信息，默认长度为16字节
    /// 超大数据发送，超出缓冲区长度的解决方案
    /// 分段发送和接收，直至匹配消息头中的数据长度
    /// 先计算是否接收到消息头长度，不足则进入下一次接收
    /// 接收完长度后开始进行消息体接收，消息体接收完成后更新接收长度标志
    /// 通过消息头确认完整数据帧
    /// 用同一个缓冲区接收消息头和消息体，只是先接收消息头，再处理消息体
    /// @todo 添加失步检测
    /// @todo 记录接收长度
    /// @todo 独立抽出反序列化器

void TransContext::on_recv()
{
    auto received_data = m_recv_buffer->try_pop(1024);
    m_frame_assembler.push_data(received_data);
    auto frame_opt = m_frame_assembler.pop_frame();
    if (!frame_opt.has_value())
    {
        return;
    }
    auto request_info_opt = Server::MessageHandler::parse_request(frame_opt.value());
    if(request_info_opt.has_value())
    {
        DaneJoe::Protocol::RequestInfo request_info = request_info_opt.value();
        DANEJOE_LOG_TRACE("default", "TransContext", "Received request: {}", request_info.url_info.to_string());
        if (request_info.url_info.path == "/download")
        {
            DANEJOE_LOG_TRACE("default", "TransContext", "Received download request");
            handle_download_request(request_info);
        }
        else if (request_info.url_info.path == "/upload")
        {
            DANEJOE_LOG_TRACE("default", "TransContext", "Received upload request");
            handle_upload_request(request_info);
        }
        else if (request_info.url_info.path == "/test")
        {
            DANEJOE_LOG_TRACE("default", "TransContext", "Received test request");
            handle_test_request(request_info);
        }
        else if (request_info.url_info.path == "/block")
        {
            DANEJOE_LOG_TRACE("default", "TransContext", "Received block request");
            handle_block_request(request_info);
        }
        else
        {
            DANEJOE_LOG_TRACE("default", "TransContext", "Received unknown request");
            handle_unknown_request(request_info);
        }
    }
}

void TransContext::on_send()
{
    // if (data.empty())
    // {
    //     return;
    // }
    // DANEJOE_LOG_TRACE("default", "TransContext", "TransContext send: {}", std::string(data.begin(), data.end()));
    // // 解析请求链接
    // auto request_info = parse_request(data);
    
}

void TransContext::handle_unknown_request(const DaneJoe::Protocol::RequestInfo& request_info)
{
    std::string response = "Recieved unknown request";
    auto test_response = Server::MessageHandler::build_test_response(response);
    m_send_buffer->push(test_response.begin(), test_response.end());
}
void TransContext::handle_download_request(const DaneJoe::Protocol::RequestInfo& request_info)
{
    auto file_info_opt = Server::MessageHandler::parse_download_request(request_info.body);
    if(!file_info_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle download request: file info not found");
        /// @todo 添加错误响应处理
        return;
    }
    // 解析下载请求消息体
    ServerFileInfo file_info = file_info_opt.value();
    DANEJOE_LOG_TRACE("default", "TransContext", "Handling download request for file: {}", file_info.to_string());
    // 构建下载响应
    std::vector<uint8_t> response = Server::MessageHandler::build_download_response(file_info);
    // 将下载响应写入发送缓冲区
    m_send_buffer->push(response.begin(), response.end());
}

void TransContext::handle_upload_request(const DaneJoe::Protocol::RequestInfo& request_info)
{
    auto file_info_opt = Server::MessageHandler::parse_upload_request(request_info.body);
    // 解析上传请求消息体
    if(!file_info_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "TransContext", "Failed to handle upload request: file info not found");
        /// @todo 添加错误响应处理
        return;
    }
    ServerFileInfo file_info = file_info_opt.value();
    /// @todo 添加文件信息到数据库
    /// @todo 上传还需要分信息和数据块
    // 构建上传响应
    auto response = Server::MessageHandler::build_upload_response(file_info);
    m_send_buffer->push(response.begin(), response.end());
}

void TransContext::handle_test_request(const DaneJoe::Protocol::RequestInfo& request_info)
{
    // 解析测试请求消息体
    std::string test_message = Server::MessageHandler::parse_test_request(request_info.body);
    DANEJOE_LOG_TRACE("default", "TransContext", "Received test request: {}", test_message);
    // 构建测试响应,当前仅做回显
    std::vector<uint8_t> response = Server::MessageHandler::build_test_response(test_message);
    m_send_buffer->push(response.begin(), response.end());
}

void TransContext::handle_block_request(const DaneJoe::Protocol::RequestInfo& request_info)
{
    // 解析块请求消息体
    auto block_info_opt = Server::MessageHandler::parse_block_request(request_info.body);
    BlockResponseInfo info = block_info_opt.value();
    // 构建块响应
    auto response = Server::MessageHandler::build_block_response(info);
    // 将块响应写入发送缓冲区
    m_send_buffer->push(response.begin(), response.end());
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

TransContext::TransContext()
{
    m_file_info_service.init();
}

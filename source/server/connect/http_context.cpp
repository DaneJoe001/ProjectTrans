#include <fstream>

#include <danejoe/logger/logger_manager.hpp>

#include "server/connect/http_context.hpp"
#include "server/connect/server_trans.hpp"

/** Http Request */
// GET / HTTP / 1.1
// Host: 127.0.0.1 : 8080
// Connection : keep - alive
// Cache - Control : max - age = 0
// sec - ch - ua : "Chromium";v = "140", "Not=A?Brand";v = "24", "Microsoft Edge";v = "140"
// sec - ch - ua - mobile: ? 0
// sec - ch - ua - platform : "Windows"
// Upgrade - Insecure - Requests : 1
// User - Agent : Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 140.0.0.0 Safari / 537.36 Edg / 140.0.0.0
// Accept : text / html, application / xhtml + xml, application / xml;q = 0.9, image / avif, image / webp, image / apng, */*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br, zstd
// Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6

HttpContext::HttpContext()
{

}
void HttpContext::on_recv()
{
    auto data = m_recv_buffer->try_pop(1024);
    if (data.empty())
    {
        return;
    }
    std::string request_str(data.begin(), data.end());
    DANEJOE_LOG_TRACE("default", "HttpContext", "Received request: {}", request_str);
    m_queue.push(data.begin(), data.end());
}
void HttpContext::on_send()
{
    auto recv_data = m_queue.try_pop(1024);
    if (recv_data.empty())
    {
        return;
    }
    std::ifstream fin("/home/danejoe001/personal_code/code_cpp_project/cpp_project_trans/resource/server/html/index.html");
    if (!fin.is_open())
    {
        return;
    }
    std::string response_body(9946, '\0');
    fin.read(response_body.data(), 9946);
    std::string response = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        response_body;
    auto send_data = std::vector<uint8_t>(response.begin(), response.end());
    m_send_buffer->push(send_data.begin(), send_data.end());
}
HttpContext::RequestInfo HttpContext::parse_request(const std::vector<uint8_t>& data)
{

}
void HttpContext::destroy(std::shared_ptr<ISocketContext> context)
{

}
void HttpContext::handle_download_request(const RequestInfo& request_info)
{

}
void HttpContext::handle_upload_request(const RequestInfo& request_info)
{

}
void HttpContext::handle_test_request(const RequestInfo& request_info)
{

}
void HttpContext::handle_block_request(const RequestInfo& request_info)
{

}

std::shared_ptr<DaneJoe::ISocketContext> HttpContextCreator::create()
{
    return std::make_shared<HttpContext>();
}
std::shared_ptr<DaneJoe::ISocketContext> HttpContextCreator::create(
    std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> recv_buffer,
    std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> send_buffer)
{
    auto context = std::make_shared<HttpContext>();
    context->set_recv_buffer(recv_buffer);
    context->set_send_buffer(send_buffer);
    ServerTrans::get_instance().register_trans(context);
    return context;
}
void HttpContextCreator::destroy(std::shared_ptr<DaneJoe::ISocketContext> context)
{
    auto trans_context = std::dynamic_pointer_cast<HttpContext>(context);
    ServerTrans::get_instance().unregister_trans(trans_context);
}
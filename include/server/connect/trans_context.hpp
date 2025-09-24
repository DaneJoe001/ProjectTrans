#pragma once

/**
 * @file trans_context.hpp
 * @brief 传输上下文
 * @author DaneJoe001
 */

#include <cstdint>
#include <unordered_map>

#include "common/network/i_socket_context.hpp"
#include "mt_queue/mt_queue.hpp"
#include "server/service/server_file_info_service.hpp"

 /**
  * @class TransContext
  * @brief 传输上下文
  */
class TransContext : public ISocketContext
{
public:
    enum class RequestType
    {
        Download,
        Upload,
        Block,
        Test
    };
    struct RequestInfo
    {
        RequestType type;
        std::unordered_map<std::string, std::string> info;
    };
public:
    TransContext();
    /**
     * @brief 接收数据回调
     * @param buffer 接收到的数据
     */
    void on_recv()override;
    /**
     * @brief 发送数据回调
     * @param buffer 发送的数据
     */
    void on_send()override;
    /**
     * @brief 解析请求
     * @param data 请求数据
     */
    RequestInfo parse_request(const std::vector<uint8_t>& data);
    /**
     * @brief 销毁传输上下文，当前仅执行解注册
     */
    void destroy(std::shared_ptr<ISocketContext> context);
    void handle_download_request(const RequestInfo& request_info);
    void handle_upload_request(const RequestInfo& request_info);
    void handle_test_request(const RequestInfo& request_info);
    void handle_block_request(const RequestInfo& request_info);
private:
    /// @brief 用于回显测试用
    DaneJoe::MTQueue<uint8_t> m_queue = DaneJoe::MTQueue<uint8_t>(4096);
    // 判断是否已接收到长度信息
    bool is_got_length_info = false;
    // 文件信息
    ServerFileInfoService m_file_info_service;
};

/**
 * @class TransContextCreator
 * @brief 传输上下文创建者
 */
class TransContextCreator : public ISocketContextCreator
{
public:
    /**
     * @brief 创建传输上下文
     * @return 传输上下文
     */
    std::shared_ptr<ISocketContext> create()override;
    /**
     * @brief 创建传输上下文
     * @param recv_buffer 接收缓冲区
     * @param send_buffer 发送缓冲区
     * @return 传输上下文
     */
    std::shared_ptr<ISocketContext> create(
        std::shared_ptr<DaneJoe::MTQueue<uint8_t>> recv_buffer,
        std::shared_ptr<DaneJoe::MTQueue<uint8_t>> send_buffer)override;
    void destroy(std::shared_ptr<ISocketContext> context)override;
};
#pragma once

/**
 * @file Http_context.hpp
 * @brief 传输上下文
 * @author DaneJoe001
 */

#include <cstdint>
#include <unordered_map>

#include "common/network/i_socket_context.hpp"
#include "mt_queue/mt_queue.hpp"

 /**
  * @class HttpContext
  * @brief 传输上下文
  */
class HttpContext : public ISocketContext
{
public:
    /**
     * @enum RequestType
     * @brief 请求类型
     */
    enum class RequestType
    {
        /// @brief 下载
        Download,
        /// @brief 上传
        Upload,
        /// @brief 块
        Block,
        /// @brief 测试
        Test
    };
    /**
     * @struct RequestInfo
     * @brief 请求信息
     */
    struct RequestInfo
    {
        /// @brief 请求类型
        RequestType type;
        /// @brief 请求信息
        std::unordered_map<std::string, std::string> info;
    };
public:
    /**
     * @brief 构造函数
     */
    HttpContext();
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
    /**
     * @brief 处理下载请求
     * @param request_info 请求信息
     */
    void handle_download_request(const RequestInfo& request_info);
    /**
     * @brief 处理上传请求
     */
    void handle_upload_request(const RequestInfo& request_info);
    /**
     * @brief 处理测试请求
     * @param request_info 请求信息
     */
    void handle_test_request(const RequestInfo& request_info);
    /**
     * @brief 处理块请求
     */
    void handle_block_request(const RequestInfo& request_info);
private:
    /// @brief 用于回echo测试用
    DaneJoe::MTQueue<uint8_t> m_queue = DaneJoe::MTQueue<uint8_t>(4096);
    // 判断是否已接收到长度信息
    bool is_got_length_info = false;
};

/**
 * @class HttpContextCreator
 * @brief 传输上下文创建者
 */
class HttpContextCreator : public ISocketContextCreator
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
    /**
     * @brief 销毁传输上下文
     * @param context 传输上下文
     */
    void destroy(std::shared_ptr<ISocketContext> context)override;
};
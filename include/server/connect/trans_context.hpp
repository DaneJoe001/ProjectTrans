#pragma once

/**
 * @file trans_context.hpp
 * @brief 传输上下文
 * @author DaneJoe001
 */

#include <cstdint>

#include "common/network/i_socket_context.hpp"

 /**
  * @class TransContext
  * @brief 传输上下文
  */
class TransContext : public ISocketContext {
public:
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
    void parse_request(const std::vector<uint8_t>& data);

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
};
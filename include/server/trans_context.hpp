#pragma once

#include "network/i_socket_context.hpp"

class TransContext : public ISocketContext {
public:
    /**
     * @brief 接收数据回调
     * @param buffer 接收到的数据
     */
    void on_recv(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)override;
    /**
     * @brief 发送数据回调
     * @param buffer 发送的数据
     */
    void on_send(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)override;
};

class TransContextCreator : public ISocketContextCreator {
public:
    std::shared_ptr<ISocketContext> create()override;
};
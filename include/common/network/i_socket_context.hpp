#pragma once

/**
 * @file i_socket_context.hpp
 * @brief 套接字上下文接口
 * @author DaneJoe001
 */
 
#include <memory>
#include <cstdint>
#include <functional>

#include "mt_queue/mt_queue.hpp"

/**
 * @class ISocketContext
 * @brief ISocketContext类实现了ISocketContext接口，用于表示套接字上下文。
 */
class ISocketContext
{
public:
    /**
     * @brief 接收数据回调
     * @param buffer 接收到的数据
     */
    virtual void on_recv(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer) = 0;
    /**
     * @brief 发送数据回调
     * @param buffer 发送的数据
     */
    virtual void on_send(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer) = 0;
};

/**
 * @class ISocketContextCreator
 * @brief ISocketContextCreator类实现了ISocketContextCreator接口，用于创建套接字上下文。
 */
class ISocketContextCreator
{
public:
    /**
     * @brief 创建套接字上下文
     * @return 套接字上下文
     */
    virtual std::shared_ptr<ISocketContext> create() = 0;
};
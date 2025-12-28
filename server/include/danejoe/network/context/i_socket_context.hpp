/**
 * @file i_socket_context.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 套接字上下文接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>
#include <cstdint>

#include "danejoe/concurrent/container/mpmc_bounded_queue.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISocketContext
     * @brief ISocketContext类实现了ISocketContext接口，用于表示套接字上下文。
     */
    class ISocketContext
    {
    public:
        /**
         * @brief 接收数据回调
         */
        virtual void on_recv() = 0;
        /**
         * @brief 发送数据回调
         */
        virtual void on_send() = 0;
        /**
         * @brief 设置接收缓冲区
         * @param buffer 接收缓冲区
         */
        void set_recv_buffer(std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> buffer);
        /**
         * @brief 设置发送缓冲区
         * @param buffer 发送缓冲区
         */
        void set_send_buffer(std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> buffer);
    protected:
        /// @brief 接收缓冲区
        std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> m_recv_buffer = nullptr;
        /// @brief 发送缓冲区
        std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> m_send_buffer = nullptr;
    };

    /**
     * @class ISocketContextCreator
     * @brief 套接字上下文创建器接口
     */
    class ISocketContextCreator
    {
    public:
        /**
         * @brief 创建套接字上下文
         * @return 套接字上下文
         */
        virtual std::shared_ptr<ISocketContext> create() = 0;
        /**
         * @brief 创建套接字上下文
         * @param recv_buffer 接收缓冲区
         * @param send_buffer 发送缓冲区
         * @return 套接字上下文
         */
        virtual std::shared_ptr<ISocketContext> create(
            std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> recv_buffer,
            std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> send_buffer) = 0;
        /**
         * @brief 解注册套接字上下文
         * @param context 套接字上下文
         */
        virtual void destroy(std::shared_ptr<ISocketContext> context) = 0;
    };

};

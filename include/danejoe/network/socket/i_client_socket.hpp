/**
 * @file i_client_socket.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 客户端套接字接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "danejoe/network/socket/i_socket.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class IClientSocket
     * @brief 客户端套接字接口
     */
    class IClientSocket : virtual public ISocket
    {
    public:
        /**
         * @brief 非阻塞读取当前连接的所有数据
         * @return 数据
         */
        virtual std::vector<uint8_t> read_all() = 0;
        /**
         * @brief 非阻塞发送所有数据
         * @param data 数据
         */
        virtual void write_all(const std::vector<uint8_t>& data) = 0;
        /**
         * @brief 阻塞直至发送完成
         * @param data 数据
         */
        virtual void write(const std::vector<uint8_t>& data) = 0;
        /**
         * @brief 接收数据至指定长度
         * @param size 数据大小
         * @return 数据
         */
        virtual std::vector<uint8_t> read(uint32_t size) = 0;
        /**
         * @brief 连接
         * @param ip ip地址
         * @param port 端口
         * @return true 连接成功
         * @return false 连接失败
         */
        virtual bool connect(const std::string& ip, uint16_t port) = 0;
        /**
         * @brief 是否已连接
         * @return true 已连接
         * @return false 未连接
         */
        virtual bool is_connected() = 0;
    protected:
        /// @brief socket
        // std::unique_ptr<ISocket> m_socket;

    };

}

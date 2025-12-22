/**
 * @file i_server_socket.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 服务器套接字接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <string>
#include <cstdint>
#include <memory>

#include "danejoe/network/socket/i_socket.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /// @brief 客户端套接字接口前向声明
    class IClientSocket;
    /**
     * @class IServerSocket
     * @brief 服务器套接字接口
     */
    class IServerSocket : virtual  public ISocket
    {
    public:
        /**
         * @brief 监听
         */
        virtual bool listen() = 0;
        /**
         * @brief 接受连接
         * @return 客户端套接字
         */
        virtual std::unique_ptr<IClientSocket> accept() = 0;
        /**
         * @brief 绑定
         * @param ip ip地址
         * @param port 端口
         */
        virtual bool bind(const std::string& ip, uint16_t port) = 0;
    };

}


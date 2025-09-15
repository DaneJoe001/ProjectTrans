#pragma once

/**
 * @file i_server_socket.hpp
 * @brief 服务器套接字接口
 * @author DaneJoe001
 */

#include <string>
#include <cstdint>
#include <memory>

#include "common/network/i_socket.hpp"

class IClientSocket;

/**
 * @class IServerSocket
 * @brief IServerSocket类实现了IServerSocket接口，用于表示服务器套接字。
 */
class IServerSocket : virtual  public ISocket {
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
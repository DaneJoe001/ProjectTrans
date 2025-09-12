#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

#include "common/network/i_socket.hpp"

/**
 * @class IClientSocket
 * @brief IClientSocket类实现了IClientSocket接口，用于表示客户端套接字。
 */
class IClientSocket : virtual public ISocket {
public:
    /**
     * @brief 非阻塞读取当前连接的所有数据
     * @return std::vector<uint8_t>
     */
    virtual std::vector<uint8_t> read_all() = 0;
    /**
     * @brief 非阻塞发送所有数据
     * @param data 数据
     */
    virtual void send_all(const std::vector<uint8_t>& data) = 0;
    /**
     * @brief 发送数据(暂时未区分是否阻塞)
     * @param data 数据
     */
    virtual void send(const std::vector<uint8_t>& data) = 0;
    /**
     * @brief 接收数据(暂时未区分是否阻塞)
     * @param size 数据大小
     * @return 数据
     */
    virtual std::vector<uint8_t> receive(std::size_t size) = 0;
    /**
     * @brief 连接
     * @param ip ip地址
     * @param port 端口
     */
    virtual bool connect(const std::string& ip, uint16_t port) = 0;
    /**
     * @brief 是否已连接
     * @return bool
     */
    virtual bool is_connected() = 0;
protected:
};
#pragma once

/**
 * @file i_client_socket.hpp
 * @brief 客户端套接字接口
 * @author DaneJoe001
 */

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
     */
    virtual bool connect(const std::string& ip, uint16_t port) = 0;
    /**
     * @brief 是否已连接
     * @return bool
     */
    virtual bool is_connected() = 0;
protected:
};
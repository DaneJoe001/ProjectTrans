#pragma once

/**
 * @file client_connection.hpp
 * @brief 客户端连接类
 * @author DaneJoe001
 */

#include <cstdint>

#include "common/network/posix_client_socket.hpp"

 /**
  * @class ClientConnection
  * @brief 客户端连接类,持有客户端Socket，并处理读写
  */
class ClientConnection
{
public:
    /**
     * @brief 构造函数
     * @param ip 服务器IP地址
     * @param port 服务器端口
     */
    ClientConnection(const std::string& ip, uint16_t port);
    /**
     * @brief 析构函数
     */
    ~ClientConnection();
    /**
     * @brief 发送数据
     * @param data 发送的数据
     */
    void send(const std::vector<uint8_t>& data);
    /**
     * @brief 接收数据
     * @return 接收的数据
     */
    std::vector<uint8_t> recieve();
    /**
     * @brief 判断是否是同一个服务器
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 是否是同一个服务器
     */
    bool is_same(const std::string& ip, uint16_t port);
    /**
     * @brief 获取服务器ID
     * @return 服务器ID
     */
    std::pair<std::string, uint16_t> get_id();
    /**
     * @brief 判断是否连接
     * @return 是否连接
     */
    bool is_connected();
    /**
     * @brief 判断是否可读
     * @return 是否可读
     */
    bool is_readable();
    /**
     * @brief 判断是否可写
     * @return 是否可写
     */
    bool is_writable();
    /**
     * @brief 获取Socket
     * @return Socket
     */
    PosixClientSocket& get_socket();
private:
    /// @brief 客户端Socket
    PosixClientSocket m_socket;
    /// @brief 服务器IP地址
    std::string m_ip;
    /// @brief 服务器端口
    uint16_t m_port;
};
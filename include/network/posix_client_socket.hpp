#pragma once

#include "network/i_client_socket.hpp"
#include "network/posix_socket.hpp"

/**
 * @class PosixClientSocket
 * @brief PosixClientSocket类实现了PosixClientSocket接口，用于表示客户端套接字。
 */
class PosixClientSocket : public IClientSocket, public PosixSocket
{
public:
    /**
     * @brief 构造函数
     * @param other 其他套接字
     */
    PosixClientSocket(PosixClientSocket&& other);
    /**
     * @brief 赋值运算符
     * @param other 其他套接字
     * @return 当前套接字
     */
    PosixClientSocket& operator=(PosixClientSocket&& other);
    /**
     * @brief 构造函数
     * @param socket 套接字
     */
    PosixClientSocket(int socket);
    /**
     * @brief 构造函数
     * @param ip ip地址
     * @param port 端口
     */
    PosixClientSocket(const std::string& ip, uint16_t  port);
    /**
     * @brief 析构函数
     */
    ~PosixClientSocket();
    /**
     * @brief 连接
     * @param ip ip地址
     * @param port 端口
     */
    bool connect(const std::string& ip, uint16_t port) override;
    /**
     * @brief 读取所有数据,使用前确保处于非阻塞模式
     * @return 数据
     */
    std::vector<uint8_t> read_all() override;
    /**
     * @brief 接收数据
     * @param size 数据大小
     * @return 数据
     */
    std::vector<uint8_t> receive(std::size_t size) override;
    /**
     * @brief 非阻塞发送所有数据
     * @param data 数据
     */
    void send_all(const std::vector<uint8_t>& data) override;
    /**
     * @brief 发送数据
     * @param data 数据
     */
    void send(const std::vector<uint8_t>& data) override;
private:
    /**
     * @brief 拷贝构造
     */
    PosixClientSocket(const PosixClientSocket&) = delete;
    /**
     * @brief 赋值运算符
     * @param other 其他套接字
     * @return 当前套接字
     */
    PosixClientSocket& operator=(const PosixClientSocket& other) = delete;
};
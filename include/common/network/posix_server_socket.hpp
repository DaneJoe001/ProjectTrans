#pragma once

#include "common/network/i_server_socket.hpp"
#include "common/network/posix_socket.hpp"

/**
 * @class PosixServerSocket
 * @brief PosixServerSocket类实现了PosixServerSocket接口，用于表示服务器套接字。
 */
class PosixServerSocket : public IServerSocket, public PosixSocket
{
public:
    /**
     * @brief 构造函数
     * @param other 其他套接字
     */
    PosixServerSocket(PosixServerSocket&& other);
    /**
     * @brief 赋值运算符
     * @param other 其他套接字
     * @return 当前套接字
     */
    PosixServerSocket& operator=(PosixServerSocket&& other);
    /**
     * @brief 构造函数
     * @param ip ip地址
     * @param port 端口
     * @param option 选项
     */
    PosixServerSocket(const std::string& ip, uint16_t  port, const IOption& option);
    /**
     * @brief 析构函数
     */
    ~PosixServerSocket();
    /**
     * @brief 监听
     */
    bool listen()override;
    /**
     * @brief 接受连接
     * @return 客户端套接字
     */
    std::unique_ptr<IClientSocket> accept()override;
    /**
     * @brief 绑定
     * @param ip ip地址
     * @param port 端口
     */
    bool bind(const std::string& ip, uint16_t port)override;
private:
    /**
     * @brief 析构函数
     */
    PosixServerSocket(const PosixServerSocket& other) = delete;
    /**
     * @brief 赋值运算符
     * @param other 其他套接字
     * @return 当前套接字
     */
    PosixServerSocket& operator=(const PosixServerSocket& other) = delete;
};
/**
 * @file posix_client_socket.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Posix客户端套接字
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#if defined(__linux__)

#include <cstdint>

#include "danejoe/network/socket/i_client_socket.hpp"
#include "danejoe/network/socket/posix_socket.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class PosixClientSocket
     * @brief Posix客户端套接字
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
        PosixClientSocket(int32_t socket);
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
         * @return true 连接成功
         * @return false 连接失败
         */
        bool connect(const std::string& ip, uint16_t port) override;
        /**
         * @brief 读取所有数据,使用前确保处于非阻塞模式
         * @return 所有数据
         */
        std::vector<uint8_t> read_all() override;
        /**
         * @brief 接收数据
         * @param size 数据大小
         * @return 指定大小的数据
         */
        std::vector<uint8_t> read(uint32_t size) override;
        /**
         * @brief 非阻塞发送所有数据
         * @param data 数据
         */
        void write_all(const std::vector<uint8_t>& data) override;
        /**
         * @brief 发送数据
         * @param data 数据
         */
        void write(const std::vector<uint8_t>& data) override;
        /**
         * @brief 是否已连接
         * @return true 已连接
         * @return false 未连接
         */
        bool is_connected()override;
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
        /// @brief 是否连接
        /// @todo 由于server在accept时没有调用connect，所以此处的m_is_connected在server端是错误的,需要考虑优化
        bool m_is_connected = true;
    };
}


#endif

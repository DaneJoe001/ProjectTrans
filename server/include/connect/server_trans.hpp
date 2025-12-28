#pragma once

/**
 * @file server_trans.hpp
 * @brief 客户端传输
 */

#include <thread>
#include <memory>
#include <unordered_set>
#include <atomic>
 #include <mutex>

#include "danejoe/network/context/i_socket_context.hpp"

 /**
  * @class ServerTrans
  * @brief 服务器端传输类，进行实际业务处理
  */
class ServerTrans
{
public:
    /**
     * @brief 获取单例
     */
    static ServerTrans& get_instance();
    /**
     * @brief 注册传输
     * @param trans 传输
     */
    bool register_trans(std::shared_ptr<DaneJoe::ISocketContext> trans);
    /**
     * @brief 注销传输
     * @param trans 传输
     */
    void unregister_trans(std::shared_ptr<DaneJoe::ISocketContext> trans);
    /**
     * @brief 接收数据处理
     */
    void recv_loop();
    /**
     * @brief 发送数据处理
     */
    void send_loop();
    /**
     * @brief 启动循环
     */
    void start_loop();
    /**
     * @brief 停止循环
     */
    void stop_loop();
private:
    /**
     * @brief 构造函数
     */
    ServerTrans();
    /**
     * @brief 析构函数
     */
    ~ServerTrans();
private:
    /// @brief 传输上下文列表
    std::unordered_set<std::shared_ptr<DaneJoe::ISocketContext>> m_trans_set;
    /// @brief 保护传输上下文列表的互斥锁（register/unregister 与 recv/send 并发）
    mutable std::mutex m_trans_mutex;
    /// @brief 是否运行
    std::atomic<bool> m_is_running = false;
    /// @brief 接收线程
    std::thread m_recv_thread;
    /// @brief 发送线程
    std::thread m_send_thread;
};
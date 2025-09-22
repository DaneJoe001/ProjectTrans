#pragma once

/**
 * @file server_trans.hpp
 * @brief 客户端传输
 */

#include <list>
#include <thread>
#include <memory>
#include <unordered_set>
#include <atomic>

#include "server/connect/trans_context.hpp"

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
    bool register_trans(std::shared_ptr<TransContext> trans);
    /**
     * @brief 注销传输
     * @param trans 传输
     */
    void unregister_trans(std::shared_ptr<TransContext> trans);
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
    ServerTrans();
    ~ServerTrans();
private:
    /// @brief 传输上下文列表
    std::unordered_set<std::shared_ptr<TransContext>> m_trans_set;
    std::atomic<bool> m_is_running = false;
    std::thread m_recv_thread;
    std::thread m_send_thread;
};
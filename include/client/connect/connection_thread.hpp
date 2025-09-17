#pragma once

/**
 * @file connection_thread.hpp
 * @brief 连接线程类
 * @author DaneJoe001
 */

#include <cstdint>
#include <memory>
#include <string>

#include <QThread>
#include <QTimer>

#include "client/connect/client_connection.hpp"

 /**
  * @class ConnectionThread
  * @brief 连接线程类
  * @note 继承自QThread，用于处理连接的读写
  */
class ConnectionThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    ConnectionThread(QObject* parent = nullptr);
    /**
     * @brief 初始化
     * @param connection 连接
     * @return 是否成功
     */
    bool init(std::unique_ptr<ClientConnection> connection);
    /**
     * @brief 初始化
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 是否成功
     */
    bool init(const std::string& ip, uint16_t port);
    /**
     * @brief 运行
     */
    void run() override;
    /**
     * @brief 反初始化,回收连接
     */
    void deinit();
    /**
     * @brief 析构函数
     */
    ~ConnectionThread();
signals:
    /**
     * @brief 数据接收信号
     * @param data 数据
     */
    void data_recieved_signal(const std::vector<uint8_t>& data);
    /**
     * @brief 数据发送槽
     * @param data 数据
     */
public slots:
    /**
     * @brief 数据发送槽
     * @param data 数据
     */
    void data_send_slot(const std::vector<uint8_t>& data);
private:
    /// @brief 连接
    std::unique_ptr<ClientConnection> m_connection = nullptr;
    /// @brief 是否运行
    bool m_is_running = false;
    /// @brief 定时器
    QTimer m_timer;
};
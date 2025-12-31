/**
 * @file network_service.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 网络服务类
 * @date 2025-12-28
 */
#pragma once

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QByteArray>

#include "common/protocol/network_endpoint.hpp"
#include "context/connect_context.hpp"

 /**
  * @class NetworkService
  * @brief 网络服务类
  * @details 管理网络连接，处理原始数据的发送和接收，负责帧的组装和分发
  */
class NetworkService :public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     */
    NetworkService();
    /**
     * @brief 析构函数
     */
    ~NetworkService();
    void init();
signals:
    /**
     * @brief 接收到完整帧信号
     * @param data 组装完成的帧数据
     */
    void received_frame_ready(QByteArray data);
public slots:
    /**
     * @brief 处理写入原始数据请求
     * @param endpoint 网络端点，指定目标地址和端口
     * @param data 要发送的原始数据
     * @details 如果端点不存在连接，则创建新的连接并发送数据；
     *          如果已存在连接，则通过现有连接发送数据
     */
    void on_write_raw_data(const NetworkEndpoint& endpoint, QByteArray data);
    /**
     * @brief 处理帧组装完成事件
     * @param frame 组装完成的帧数据
     * @details 当连接上下文完成帧组装后，通过此槽函数接收并转发帧数据
     */
    void on_frame_assembled(QByteArray frame);
private:
    /// @brief 连接映射表，按网络端点组织连接上下文列表
    std::unordered_map<NetworkEndpoint, std::vector<std::unique_ptr<ConnectContext>>> m_connect_map;
};
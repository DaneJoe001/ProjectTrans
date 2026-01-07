/**
  * @file connect_context.hpp
  * @brief 连接上下文
  * @author DaneJoe001
  * @date 2026-01-06
  * @details 管理单个 QTcpSocket 的读写状态，提供写缓冲，并对接收数据进行帧组装。
  */
#pragma once

#include <QObject>
#include <QPointer>
#include <QTcpSocket>

#include "danejoe/network/codec/frame_assembler.hpp"

/**
  * @class ConnectContext
  * @brief 连接上下文
  * @details 绑定到一个 QTcpSocket，用于：
  *          1) 维护写缓冲并在可写时发送
  *          2) 将接收数据推入 FrameAssembler 并在组装出完整帧后发出信号
  */
class ConnectContext : public QObject
{
    Q_OBJECT
public:
    /**
      * @brief 构造函数
      * @param socket 需要绑定的 socket
      * @param parent Qt 父对象
      * @details 构造时会连接 socket 的读写/断开等信号，以驱动内部状态更新与帧组装。
      */
    ConnectContext(QTcpSocket* socket, QObject* parent = nullptr);
    /**
      * @brief 写入数据
      * @param data 待发送的数据
      * @details 数据会先追加到写缓冲；当 socket 可写时会尝试发送，并从缓冲中移除已发送部分。
      */
    void write_data(QByteArray data);
    /**
      * @brief 设置 socket
      * @param socket 需要绑定的新 socket
      */
    void set_socket(QTcpSocket* socket);
signals:
    /**
      * @brief 完整帧组装完成信号
      * @param frame 组装完成的帧数据
      */
    void frame_assembled(QByteArray frame);
    /**
      * @brief socket 断开连接信号
      */
    void socket_disconnected();
private slots:
    /**
      * @brief 处理 socket 写入相关事件
      * @details 当 socket 连接成功或写入完成时触发，用于继续发送写缓冲中的剩余数据。
      */
    void on_socket_write();
    /**
      * @brief 处理 socket 读事件
      * @details 读取 socket 的可用数据，并尝试从 FrameAssembler 中弹出完整帧。
      */
    void on_socket_read();
    /**
      * @brief 处理 socket 断开事件
      */
    void on_socket_disconnected();
private:
    /// @brief 绑定的 socket 指针（QPointer 自动跟踪对象销毁）
    QPointer<QTcpSocket> m_socket;
    /// @brief 待发送的写缓冲
    QByteArray m_write_buffer;
    /// @brief 帧组装器，用于将接收字节流切分为完整帧
    DaneJoe::FrameAssembler m_frame_assembler;
    /// @brief 最近一次读写活动时间
    std::chrono::steady_clock::time_point m_last_activity;
};
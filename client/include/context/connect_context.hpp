#pragma once

#include <QObject>
#include <QPointer>
#include <QTcpSocket>

#include "danejoe/network/codec/frame_assembler.hpp"

class ConnectContext : public QObject
{
    Q_OBJECT
public:
    ConnectContext(QTcpSocket* socket, QObject* parent = nullptr);
    void write_data(QByteArray data);
    void set_socket(QTcpSocket* socket);
signals:
    void frame_assembled(QByteArray frame);
    void socket_disconnected();
private slots:
    void on_socket_write();
    void on_socket_read();
    void on_socket_disconnected();
private:
    QPointer<QTcpSocket> m_socket;
    QByteArray m_write_buffer;
    DaneJoe::FrameAssembler m_frame_assembler;
    std::chrono::steady_clock::time_point m_last_activity;
};
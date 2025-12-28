#pragma once

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QByteArray>

#include "danejoe/network/codec/frame_assembler.hpp"
#include "common/protocol/network_endpoint.hpp"

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

class NetworkService :public QObject
{
    Q_OBJECT
public:
    NetworkService();
    ~NetworkService();
signals:
    void received_frame_ready(QByteArray data);
public slots:
    void on_write_raw_data(const NetworkEndpoint& endpoint, QByteArray data);
    void on_frame_assembled(QByteArray frame);
private:
    std::unordered_map<NetworkEndpoint, std::vector<std::unique_ptr<ConnectContext>>> m_connect_map;
};
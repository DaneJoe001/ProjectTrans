#pragma once

#include <cstdint>
#include <string>
#include <atomic>
#include <vector>
#include <chrono>
#include <memory>
#include <unordered_map>

#include <QObject>
#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QByteArray>

#include "danejoe/network/url/url_info.hpp"
#include "danejoe/network/codec/frame_assembler.hpp"

#include "client/protocol/client_message_codec.hpp"

struct Endpoint
{
    std::string ip;
    uint16_t port;
    bool operator==(const Endpoint& other) const
    {
        return ip == other.ip && port == other.port;
    }
};

namespace std
{
    template<>
    struct hash<Endpoint>
    {
        std::size_t operator()(const Endpoint& endpoint) const noexcept
        {
            std::size_t h1 = std::hash<std::string>{}(endpoint.ip);
            std::size_t h2 = std::hash<uint16_t>{}(endpoint.port);
            return h1 ^ (h2 << 1);
        }
    };
}

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
    void on_write_raw_data(const Endpoint& endpoint, QByteArray data);
    void on_frame_assembled(QByteArray frame);
private:
    std::unordered_map<Endpoint, std::vector<std::unique_ptr<ConnectContext>>> m_connect_map;
};

class TransService : public QObject
{
    Q_OBJECT
public:
signals:
    void test_response_received(std::string response);
public:
    TransService();
    ~TransService();
    void send_test_request(const Endpoint& endpoint, const std::string& message);
    void send_download_request(const Endpoint& endpoint, const std::string& message);
    void send_block_request(const Endpoint& endpoint, const std::string& message);
    void receive_test_response(const std::vector<uint8_t>& data);
    void receive_download_response(const std::vector<uint8_t>& data);
    void receive_block_response(const std::vector<uint8_t>& data);
signals:
    void send_frame_ready(QByteArray data);
public slots:
    void on_received_frame_ready(QByteArray data);
public:
private:
    ClientMessageCodec m_message_codec;
    std::atomic<uint64_t> m_request_id_counter = 0;
    QThread* m_network_thread = nullptr;
    NetworkService* m_network_service = nullptr;
};
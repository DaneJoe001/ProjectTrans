#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <QObject>
#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QByteArray>

#include "danejoe/concurrent/timer/timer_manager.hpp"

#include "service/network_service.hpp"
#include "protocol/client_message_codec.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "context/trans_context.hpp"

struct TransCorrelation
{
    TransContext context;
    std::function<void(std::vector<uint8_t>)> callback;
};

class TransService : public QObject
{
    Q_OBJECT
public:
    void init();
    static TransService& get_instance();
    TransContext send_test_request(
        const NetworkEndpoint& endpoint,
        const TestRequestTransfer& request);
    TransContext send_download_request(
        const NetworkEndpoint& endpoint,
        const DownloadRequestTransfer& request);
    TransContext send_block_request(
        const NetworkEndpoint& endpoint,
        const BlockRequestTransfer& request);
signals:
    void test_response_received(
        TransContext trans_context,
        TestResponseTransfer response);
    void download_response_received(
        TransContext trans_context,
        DownloadResponseTransfer response);
    void block_response_received(
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    TransService();
    ~TransService();
    void receive_test_response(
        uint64_t request_id,
        const std::vector<uint8_t>& data);
    void receive_download_response(
        uint64_t request_id,
        const std::vector<uint8_t>& data);
    void receive_block_response(
        uint64_t request_id,
        const std::vector<uint8_t>& data);
    void remove_response_handler(uint64_t request_id);
signals:
    void send_frame_ready(QByteArray data);
public slots:
    void on_received_frame_ready(QByteArray data);
public:
private:
    std::mutex m_mutex;
    std::chrono::steady_clock::duration m_time_out_interval;
    DaneJoe::TimerManager& m_timer_manager;
    std::unordered_map<uint64_t, TransCorrelation> m_trans_correlations;
    ClientMessageCodec m_message_codec;
    std::atomic<uint64_t> m_request_id_counter = 0;
    QThread* m_network_thread = nullptr;
    NetworkService* m_network_service = nullptr;
};
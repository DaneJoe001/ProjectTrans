#pragma once

#include <atomic>

#include <QObject>

#include "common/protocol/network_endpoint.hpp"
#include "view/event/event_envelope.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "context/trans_context.hpp"

class ViewEventHub : public QObject
{
    Q_OBJECT
public:
    void init();
    void publish_test_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    void publish_block_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    void publish_download_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    void publish_test_response(
        EventEnvelope event_envelope,
        TransContext trans_context,
        TestResponseTransfer response);
signals:
    void test_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    void block_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    void download_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    void test_response(
        EventEnvelope event_envelope,
        TransContext trans_context,
        TestResponseTransfer response);
private:
    std::atomic<uint64_t> m_event_id_counter = 0;
};
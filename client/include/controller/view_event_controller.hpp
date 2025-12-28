#pragma once

#include <unordered_map>

#include <QObject>
#include <QPointer>

#include "common/protocol/network_endpoint.hpp"
#include "view/event/event_envelope.hpp"
#include "view/event/view_event_hub.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "service/trans_service.hpp"

class ViewEventController : public QObject
{
public:
    Q_OBJECT
public:
    ViewEventController(
        QPointer<ViewEventHub> event_hub,
        TransService& trans_service,
        QObject* parent = nullptr);
    void init();
signals:

public slots:
    void on_test_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    void on_block_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    void on_download_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    void on_test_response(
        TransContext trans_context,
        TestResponseTransfer response);
private:
    QPointer<ViewEventHub> m_view_event_hub;
    TransService& m_trans_service;
    std::unordered_map<uint64_t, EventEnvelope> m_event_envelopes;
};
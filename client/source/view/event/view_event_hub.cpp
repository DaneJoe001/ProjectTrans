#include <chrono>

#include "view/event/view_event_hub.hpp"

void ViewEventHub::init()
{

}
void ViewEventHub::publish_test_request(
    EventContext event_context,
    NetworkEndpoint endpoint,
    TestRequestTransfer request)
{
    EventEnvelope event_envelope;
    event_envelope.m_event_context = event_context;
    event_envelope.m_event_id = m_event_id_counter++;
    event_envelope.m_event_time = std::chrono::steady_clock::now();
    emit test_request(event_envelope, endpoint, request);
}
void ViewEventHub::publish_block_request(
    EventContext event_context,
    NetworkEndpoint endpoint,
    BlockRequestTransfer request)
{
    EventEnvelope event_envelope;
    event_envelope.m_event_context = event_context;
    event_envelope.m_event_id = m_event_id_counter++;
    event_envelope.m_event_time = std::chrono::steady_clock::now();
    emit block_request(event_envelope, endpoint, request);
}
void ViewEventHub::publish_download_request(
    EventContext event_context,
    NetworkEndpoint endpoint,
    DownloadRequestTransfer request)
{
    EventEnvelope event_envelope;
    event_envelope.m_event_context = event_context;
    event_envelope.m_event_id = m_event_id_counter++;
    event_envelope.m_event_time = std::chrono::steady_clock::now();
    emit download_request(event_envelope, endpoint, request);
}

void ViewEventHub::publish_test_response(
    EventEnvelope event_envelope,
    TransContext trans_context,
    TestResponseTransfer response)
{
    emit test_response(event_envelope, trans_context, response);
}
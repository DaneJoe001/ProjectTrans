#include "controller/view_event_controller.hpp"

void ViewEventController::init()
{

}

void ViewEventController::on_test_request(
    EventEnvelope event_envelope,
    NetworkEndpoint endpoint,
    TestRequestTransfer request)
{
    auto trans_context = m_trans_service.send_test_request(endpoint, request);
    m_event_envelopes[trans_context.request_id] = event_envelope;
}
void ViewEventController::on_block_request(
    EventEnvelope event_envelope,
    NetworkEndpoint endpoint,
    BlockRequestTransfer request)
{
    auto trans_context = m_trans_service.send_block_request(endpoint, request);
    m_event_envelopes[trans_context.request_id] = event_envelope;
}
void ViewEventController::on_download_request(
    EventEnvelope event_envelope,
    NetworkEndpoint endpoint,
    DownloadRequestTransfer request)
{
    auto trans_context = m_trans_service.send_download_request(endpoint, request);
    m_event_envelopes[trans_context.request_id] = event_envelope;
}

void ViewEventController::on_test_response(
    TransContext trans_context,
    TestResponseTransfer response)
{
    auto event_envelope_it = m_event_envelopes.find(trans_context.request_id);
    if (event_envelope_it == m_event_envelopes.end())
    {
        return;
    }
    m_view_event_hub->publish_test_response(
        event_envelope_it->second,
        trans_context,
        response);
}
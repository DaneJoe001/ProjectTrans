#include "controller/view_event_controller.hpp"

ViewEventController::ViewEventController(
    QPointer<ViewEventHub> event_hub,
    TransService& trans_service,
    QObject* parent)
    : QObject(parent),
      m_view_event_hub(event_hub),
      m_trans_service(trans_service)
{
    init();
}

void ViewEventController::init()
{
    if (m_view_event_hub)
    {
        connect(m_view_event_hub, &ViewEventHub::test_request,
            this, &ViewEventController::on_test_request);
        connect(m_view_event_hub, &ViewEventHub::block_request,
            this, &ViewEventController::on_block_request);
        connect(m_view_event_hub, &ViewEventHub::download_request,
            this, &ViewEventController::on_download_request);
    }
    connect(&m_trans_service, &TransService::test_response_received, this,
            &ViewEventController::on_test_response);
    connect(&m_trans_service, &TransService::download_response_received, this,
            &ViewEventController::on_download_response);
    connect(&m_trans_service, &TransService::block_response_received, this,
            &ViewEventController::on_block_response);
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
void ViewEventController::on_download_response(
    TransContext trans_context, DownloadResponseTransfer response)
{
  auto event_envelope_it = m_event_envelopes.find(trans_context.request_id);
  if (event_envelope_it == m_event_envelopes.end())
  {
    return;
  }
  m_view_event_hub->publish_download_response(
      event_envelope_it->second,
      trans_context,
      response);
}

void ViewEventController::on_block_response(TransContext trans_context,
                                            BlockResponseTransfer response)
{
  auto event_envelope_it = m_event_envelopes.find(trans_context.request_id);
  if (event_envelope_it == m_event_envelopes.end())
  {
    return;
  }
  m_view_event_hub->publish_block_response(
      event_envelope_it->second,
      trans_context,
      response);
}
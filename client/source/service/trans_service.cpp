#include "danejoe/logger/logger_manager.hpp"
#include "service/trans_service.hpp"

using namespace std::chrono_literals;

TransService& TransService::get_instance()
{
    static TransService instance;
    return instance;
}
TransService::TransService() :
    m_time_out_interval(10s),
    m_timer_manager(DaneJoe::TimerManager::get_instance())
{
    m_network_thread = new QThread(this);
    m_network_service = new NetworkService();
    m_network_service->moveToThread(m_network_thread);
    m_network_thread->start();
    connect(m_network_service, &NetworkService::received_frame_ready, this, &TransService::on_received_frame_ready);
}
TransService::~TransService()
{
    m_network_service->deleteLater();
    m_network_thread->quit();
    m_network_thread->wait();
    m_network_thread->deleteLater();
}
TransContext TransService::send_test_request(
    const NetworkEndpoint& endpoint,
    const TestRequestTransfer& request)
{
    uint64_t request_id = m_request_id_counter++;
    m_timer_manager.add_task_for(m_time_out_interval,
        [this, request_id]()
        {
            if (m_trans_correlations.find(request_id) != m_trans_correlations.end())
            {
                DANEJOE_LOG_WARN("default", "TransService", "Timeout for request id {}", request_id);
                this->remove_response_handler(request_id);
            }
        });
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        TransCorrelation correlation;
        correlation.context = TransContext{ request_id,endpoint };
        correlation.callback = [this, request_id](const std::vector<uint8_t>& data)
            {
                receive_test_response(request_id, data);
            };
        m_trans_correlations[request_id] = correlation;
    }
    auto data = m_message_codec.build_test_request(request, request_id);
    m_network_service->on_write_raw_data(endpoint, QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    return TransContext{ request_id,endpoint };
}
TransContext TransService::send_download_request(
    const NetworkEndpoint& endpoint,
    const DownloadRequestTransfer& request)
{
    uint64_t request_id = m_request_id_counter++;
    m_timer_manager.add_task_for(m_time_out_interval,
        [this, request_id]()
        {
            if (m_trans_correlations.find(request_id) != m_trans_correlations.end())
            {
                DANEJOE_LOG_WARN("default", "TransService", "Timeout for request id {}", request_id);
                this->remove_response_handler(request_id);
            }
        }
    );
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        TransCorrelation correlation;
        correlation.context = TransContext{ request_id,endpoint };
        correlation.callback = [this, request_id](const std::vector<uint8_t>& data)
            {
                receive_download_response(request_id, data);
            };
        m_trans_correlations[request_id] = correlation;
    }
    auto data = m_message_codec.build_download_request(request, request_id);
    m_network_service->on_write_raw_data(endpoint, QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    return TransContext{ request_id,endpoint };
}
TransContext TransService::send_block_request(
    const NetworkEndpoint& endpoint,
    const BlockRequestTransfer& request)
{
    uint64_t request_id = m_request_id_counter++;
    m_timer_manager.add_task_for(m_time_out_interval,
        [this, request_id]()
        {
            if (m_trans_correlations.find(request_id) != m_trans_correlations.end())
            {
                DANEJOE_LOG_WARN("default", "TransService", "Timeout for request id {}", request_id);
                this->remove_response_handler(request_id);
            }
        }
    );
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        TransCorrelation correlation;
        correlation.context = TransContext{ request_id,endpoint };
        correlation.callback = [this, request_id](const std::vector<uint8_t>& data)
            {
                receive_block_response(request_id, data);
            };
        m_trans_correlations[request_id] = correlation;
    }
    auto data = m_message_codec.build_block_request(request, request_id);
    m_network_service->on_write_raw_data(endpoint, QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    return TransContext{ request_id,endpoint };
}
void TransService::receive_test_response(
    uint64_t request_id,
    const std::vector<uint8_t>& data)
{
    auto test_response_opt =
        m_message_codec.
        try_parse_test_response(data);
    if (!test_response_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to parse test response");
        return;
    }
    auto response =
        std::move(test_response_opt.value());
    auto trans_correlation_it = m_trans_correlations.find(request_id);
    if (trans_correlation_it == m_trans_correlations.end())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to find trans correlation");
        return;
    }
    emit test_response_received(trans_correlation_it->second.context, response);
}
void TransService::receive_download_response(
    uint64_t request_id,
    const std::vector<uint8_t>& data)
{
    auto download_response_opt =
        m_message_codec.
        try_parse_download_response(data);
    if (!download_response_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to parse download response");
        return;
    }
    auto response =
        std::move(download_response_opt.value());
    auto trans_correlation_it = m_trans_correlations.find(request_id);
    if (trans_correlation_it == m_trans_correlations.end())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to find trans correlation");
        return;
    }
    emit download_response_received(trans_correlation_it->second.context, response);
}
void TransService::receive_block_response(
    uint64_t request_id,
    const std::vector<uint8_t>& data)
{
    auto block_response_opt =
        m_message_codec.
        try_parse_block_response(data);
    if (!block_response_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to parse block response");
        return;
    }
    auto response =
        std::move(block_response_opt.value());
    auto trans_correlation_it = m_trans_correlations.find(request_id);
    if (trans_correlation_it == m_trans_correlations.end())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to find trans correlation");
        return;
    }
    emit block_response_received(trans_correlation_it->second.context, response);
}

void TransService::on_received_frame_ready(QByteArray data)
{
    DANEJOE_LOG_DEBUG("default", "TransService", "on_received_frame_ready");
    auto response_opt =
        m_message_codec.try_parse_response(std::vector<uint8_t>(data.begin(), data.end()));
    if (!response_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "TransService", "Failed to parse response");
        return;
    }
    auto response =
        std::move(response_opt.value());
    auto handler_it =
        m_trans_correlations.find(response.request_id);
    if (handler_it == m_trans_correlations.end())
    {
        DANEJOE_LOG_WARN("default", "TransService", "No handler found for request id {}", response.request_id);
        return;
    }
    auto handler = std::move(handler_it->second.callback);
    remove_response_handler(response.request_id);
    handler(response.body);
}

void TransService::remove_response_handler(uint64_t request_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_trans_correlations.erase(request_id);
}
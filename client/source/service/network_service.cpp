#include "danejoe/logger/logger_manager.hpp"
#include "service/network_service.hpp"

NetworkService::NetworkService() {}
NetworkService::~NetworkService() {}

void NetworkService::init()
{

}

void NetworkService::on_write_raw_data(const NetworkEndpoint& endpoint, QByteArray data)
{
    auto connect_context_it = m_connect_map.find(endpoint);
    if (connect_context_it == m_connect_map.end())
    {
        auto socket = new QTcpSocket(this);
        auto connect_context = std::make_unique<ConnectContext>(socket, this);
        connect_context->write_data(data);
        connect(connect_context.get(), &ConnectContext::frame_assembled, this, &NetworkService::on_frame_assembled);
        socket->connectToHost(QString::fromStdString(endpoint.ip), endpoint.port);
        m_connect_map[endpoint].push_back(std::move(connect_context));
    }
    else
    {
        /// @todo 如何处理负载问题
        connect_context_it->second.front()->write_data(data);
    }
}

void NetworkService::on_frame_assembled(QByteArray frame)
{
    emit received_frame_ready(frame);
}
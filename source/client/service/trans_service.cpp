#include "danejoe/logger/logger_manager.hpp"
#include "danejoe/network/codec/serialize_codec.hpp"

#include "client/service/trans_service.hpp"

ConnectContext::ConnectContext(QTcpSocket* socket, QObject* parent) : QObject(parent), m_socket(socket)
{
    connect(m_socket, &QTcpSocket::connected, this, &ConnectContext::on_socket_write);
    connect(m_socket, &QTcpSocket::readyRead, this, &ConnectContext::on_socket_read);
    connect(m_socket, &QTcpSocket::disconnected, this, &ConnectContext::on_socket_disconnected);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &ConnectContext::on_socket_write);
}

void ConnectContext::write_data(QByteArray data)
{
    m_last_activity = std::chrono::steady_clock::now();
    m_write_buffer.append(data);
    if (!m_socket || !m_socket->isOpen() || !m_socket->isWritable())
    {
        return;
    }
    auto written_size = m_socket->write(m_write_buffer);
    if (written_size < 0)
    {
        DANEJOE_LOG_ERROR("default", "ConnectContext", "Failed to write data to socket: {}", m_socket->errorString().toStdString());
        return;
    }
    else
    {
        m_write_buffer.remove(0, written_size);
    }

}
void ConnectContext::set_socket(QTcpSocket* socket)
{
    m_socket = socket;
}
void ConnectContext::on_socket_write()
{
    write_data(QByteArray());
}

void ConnectContext::on_socket_read()
{
    if (!m_socket)
    {
        return;
    }
    DANEJOE_LOG_DEBUG("default", "ConnectContext", "Socket ready read");
    auto data = m_socket->readAll();
    std::vector<uint8_t> vec(data.begin(), data.end());
    m_frame_assembler.push_data(vec);
    while (auto frame_opt = m_frame_assembler.pop_frame())
    {
        if (frame_opt.has_value())
        {
            QByteArray frame_data(reinterpret_cast<const char*>(frame_opt.value().data()), frame_opt.value().size());
            emit frame_assembled(frame_data);
        }
    }
}

void ConnectContext::on_socket_disconnected()
{
    emit socket_disconnected();
}

NetworkService::NetworkService() {}
NetworkService::~NetworkService() {}
void NetworkService::on_write_raw_data(const Endpoint& endpoint, QByteArray data)
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

TransService::TransService()
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
void TransService::send_test_request(const Endpoint& endpoint, const std::string& message)
{
    DaneJoe::UrlInfo url_info;
    url_info.protocol = DaneJoe::UrlProtocol::Danejoe;
    auto data = m_message_codec.build_test_request(url_info, message);
    m_network_service->on_write_raw_data(endpoint, QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
}
void TransService::send_download_request(const Endpoint& endpoint, const std::string& message)
{

}
void TransService::send_block_request(const Endpoint& endpoint, const std::string& message)
{

}
void TransService::receive_test_response(const std::vector<uint8_t>& data)
{
    auto response = m_message_codec.parse_test_response(data);
    DANEJOE_LOG_INFO("default", "TransService", "Received test response: {}", response);
    emit test_response_received(response);
}
void TransService::receive_download_response(const std::vector<uint8_t>& data)
{

}
void TransService::receive_block_response(const std::vector<uint8_t>& data)
{

}

void TransService::on_received_frame_ready(QByteArray data)
{
    DANEJOE_LOG_DEBUG("default", "TransService", "on_received_frame_ready");
    auto response_opt = m_message_codec.parse_response(std::vector<uint8_t>(data.begin(), data.end()));
    if (response_opt.has_value())
    {
        receive_test_response(response_opt->body);
    }
}
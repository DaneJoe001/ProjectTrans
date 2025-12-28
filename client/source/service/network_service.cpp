#include "danejoe/logger/logger_manager.hpp"
#include "service/network_service.hpp"

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
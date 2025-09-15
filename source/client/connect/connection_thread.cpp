#include "client/connect/connection_thread.hpp"
#include "client/connect/connection_manager.hpp"
#include "log/manage_logger.hpp"

ConnectionThread::ConnectionThread(QObject* parent) :QThread(parent)
{

}

bool ConnectionThread::init(std::unique_ptr<ClientConnection> connection)
{
    m_connection = std::move(connection);
    return m_connection != nullptr && m_connection->is_connected();
}
bool ConnectionThread::init(const std::string& ip, uint16_t port)
{
    auto& manager = ConnectionManager::get_instance();
    manager.add_connection(ip, port);
    m_connection = manager.get_connection(ip, port);
    return m_connection != nullptr && m_connection->is_connected();
}
void ConnectionThread::run()
{
    if (m_is_running)
    {
        return;
    }
    m_is_running = true;
    while (m_is_running)
    {
        if (m_connection && m_connection->is_connected())
        {
            /// @brief 当前为非阻塞实现
            auto data = m_connection->recieve();
            if (!data.empty())
            {
                emit data_recieved_signal(data);
            }
        }
        QThread::msleep(100);
    }
}

void ConnectionThread::stop()
{
    m_is_running = false;
    wait();
    ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
}

ConnectionThread::~ConnectionThread()
{
    stop();
}

void ConnectionThread::data_send_slot(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ConnectionThread", "data_send_slot");
    if (m_connection && m_connection->is_connected())
    {
        m_connection->send(data);
    }
}
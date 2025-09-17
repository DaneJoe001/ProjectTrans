#include "client/connect/connection_thread.hpp"
#include "client/connect/connection_manager.hpp"
#include "log/manage_logger.hpp"

ConnectionThread::ConnectionThread(QObject* parent) :QThread(parent)
{

}

bool ConnectionThread::init(std::unique_ptr<ClientConnection> connection)
{
    // 当内部连接不为空时先进行释放
    if (m_connection)
    {
        deinit();
    }
    // 通过直接转移连接进行初始化
    m_connection = std::move(connection);
    // 确认连接非nullptr且连接正常
    return m_connection != nullptr && m_connection->is_connected();
}
bool ConnectionThread::init(const std::string& ip, uint16_t port)
{
    // 当内部连接不为空时先进行释放
    if (m_connection)
    {
        deinit();
    }
    // 获取连接管理器
    auto& manager = ConnectionManager::get_instance();
    // 添加对应连接
    manager.add_connection(ip, port);
    // 获取连接
    m_connection = manager.get_connection(ip, port);
    // 确认连接非nullptr且连接正常
    return m_connection != nullptr && m_connection->is_connected();
}
void ConnectionThread::run()
{
    // 设置连接标志为true
    m_is_running = true;
    while (m_is_running)
    {
        // 接收前先判断可用性
        if (m_connection && m_connection->is_connected())
        {
            // 当前为非阻塞实现
            auto data = m_connection->recieve();
            // 当接收到的数据不为空时发送接收到数据的信号
            if (!data.empty())
            {
                emit data_recieved_signal(data);
            }
        }
        // 延时等待，避免忙等待
        QThread::msleep(100);
    }
}

void ConnectionThread::deinit()
{
    // 回收内部连接
    ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
    // 置空内部连接
    m_connection = nullptr;
}

ConnectionThread::~ConnectionThread()
{
    // 由于需要维持线程运行，故只在析构中更新运行标志
    m_is_running = false;
    // 等待线程结束
    wait();
    // 回收连接
    deinit();
}

void ConnectionThread::data_send_slot(const std::vector<uint8_t>& data)
{
    DANEJOE_LOG_TRACE("default", "ConnectionThread", "data_send_slot");
    // 当连接可用时发送数据
    /// @todo 实现消息缓冲区，避免信息丢失
    if (m_connection && m_connection->is_connected())
    {
        m_connection->send(data);
    }
}
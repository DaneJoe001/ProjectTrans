#include <thread>
#include <mutex>
#include <vector>

#include <danejoe/logger/logger_manager.hpp>

#include "server/connect/server_trans.hpp"

ServerTrans& ServerTrans::get_instance()
{
    // 定义实例
    static ServerTrans instance;
    return instance;
}

ServerTrans::ServerTrans()
{

}
ServerTrans::~ServerTrans()
{
    if (m_is_running.load())
    {
        stop_loop();
    }
}

bool ServerTrans::register_trans(std::shared_ptr<DaneJoe::ISocketContext> trans)
{
    // 在有效的情况下插入set
    if (!trans)
    {
        DANEJOE_LOG_ERROR("default", "ServerTrans", "Failed to register trans: trans is null");
        return false;
    }
    std::lock_guard<std::mutex> lock(m_trans_mutex);
    m_trans_set.insert(trans);
    return true;
}

void ServerTrans::unregister_trans(std::shared_ptr<DaneJoe::ISocketContext> trans)
{
    std::lock_guard<std::mutex> lock(m_trans_mutex);
    auto it = m_trans_set.find(trans);
    if (it == m_trans_set.end())
    {
        DANEJOE_LOG_TRACE("default", "ServerTrans", "Failed to unregister trans: trans is not found");
        return;
    }
    m_trans_set.erase(it);
}

void ServerTrans::recv_loop()
{
    while (m_is_running.load())
    {
        std::vector<std::shared_ptr<DaneJoe::ISocketContext>> snapshot;
        {
            std::lock_guard<std::mutex> lock(m_trans_mutex);
            snapshot.reserve(m_trans_set.size());
            for (auto& trans : m_trans_set)
            {
                snapshot.push_back(trans);
            }
        }

        for (auto& trans : snapshot)
        {
            if (!trans)
            {
                DANEJOE_LOG_WARN("default", "ServerTrans", "Failed to recv: trans is null");
                continue;
            }
            trans->on_recv();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ServerTrans::send_loop()
{
    while (m_is_running.load())
    {
        std::vector<std::shared_ptr<DaneJoe::ISocketContext>> snapshot;
        {
            std::lock_guard<std::mutex> lock(m_trans_mutex);
            snapshot.reserve(m_trans_set.size());
            for (auto& trans : m_trans_set)
            {
                snapshot.push_back(trans);
            }
        }

        for (auto& trans : snapshot)
        {
            if (!trans)
            {
                DANEJOE_LOG_WARN("default", "ServerTrans", "Failed to send: trans is null");
                continue;
            }
            trans->on_send();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ServerTrans::start_loop()
{
    m_is_running.store(true);
    m_recv_thread = std::move(std::thread(&ServerTrans::recv_loop, this));
    m_send_thread = std::move(std::thread(&ServerTrans::send_loop, this));
}

void ServerTrans::stop_loop()
{
    m_is_running.store(false);
    if (m_recv_thread.joinable())
    {
        m_recv_thread.join();
    }
    if (m_send_thread.joinable())
    {
        m_send_thread.join();
    }
}
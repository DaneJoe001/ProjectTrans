#include "client/connect/connection_manager.hpp"
#include "client/connect/trans_manager.hpp"
#include "log/manage_logger.hpp"

TransManager::TransManager(QObject* parent) :QObject(parent), m_connection_manager(ConnectionManager::get_instance())
{

}

TransManager::~TransManager()
{
    m_is_running.store(false);
    m_client_file_info_queue.close();
}

void TransManager::add_trans(const ClientFileInfo& info)
{
    /// @todo 实现try_push，避免满阻塞
    m_client_file_info_queue.push(info);
}

void TransManager::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TransManager", "TransManager has been initialized");
        return;
    }
    m_block_request_info_service.init();
    m_is_running = true;
    m_client_file_info_queue = DaneJoe::MTQueue<ClientFileInfo>(128);
    add_thread();
    m_client_file_info_thread = std::thread([&]()
        {
            while (m_is_running.load())
            {
                auto client_file_info_option = m_client_file_info_queue.pop();
                if (!client_file_info_option.has_value())
                {
                    continue;
                }
                auto client_file_info = client_file_info_option.value();
                auto block_info_list = m_block_request_info_service.get_by_file_id_and_state(client_file_info.file_id, FileState::Waiting);
                for (auto& block_info : block_info_list)
                {
                    auto m_block_task_queue_it = m_block_task_queue_list.begin();
                    if (m_block_task_queue_it == m_block_task_queue_list.end())
                    {
                        DANEJOE_LOG_ERROR("default", "TransManager", "No block task queue");
                        continue;
                    }
                    (*m_block_task_queue_it)->push(block_info);
                }
            }
        });
    m_is_init = true;
}

void TransManager::add_thread()
{
    if (m_thread_count >= m_max_thread_count)
    {
        DANEJOE_LOG_TRACE("default", "TransManager", "Failed to add thread: thread count is full");
        return;
    }
    // 块信息任务队列
    std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> block_task_queue = std::make_shared<DaneJoe::MTQueue<BlockRequestInfo>>(1024);
    m_block_task_queue_list.push_back(block_task_queue);
    BlockRequestThread* block_request_thread = new BlockRequestThread(this);
    m_block_thread_list.push_back(block_request_thread);
    block_request_thread->init(block_task_queue);
    block_request_thread->start();
    m_thread_count++;
}
#include "client/connect/connection_manager.hpp"
#include "client/connect/trans_manager.hpp"
#include "log/manage_logger.hpp"

TransManager::TransManager(QObject* parent) :QObject(parent), m_connection_manager(ConnectionManager::get_instance())
{

}

TransManager::~TransManager()
{
    // 更新文件信息处理线程标志位
    m_is_running.store(false);
    // 关闭文件信息队列
    m_client_file_info_queue.close();
    // 停止所有块信息队列
    for (auto& block_task_queue : m_block_task_queue_list)
    {
        // 检查队列是否为空
        if (block_task_queue)
        {
            block_task_queue->close();
        }
    }
    // 停止所有块处理线程
    for (auto& block_request_thread : m_block_thread_list)
    {
        // 检查线程是否初始化
        if (block_request_thread)
        {
            block_request_thread->stop();
        }
    }
}

void TransManager::add_trans(const ClientFileInfo& info)
{
    if (!m_client_file_info_queue.is_running())
    {
        DANEJOE_LOG_ERROR("default", "TransManager", "Failed to add trans, queue is not running");
        return;
    }
    /// @todo 实现try_push，避免满阻塞
    m_client_file_info_queue.push(info);
    // 更新文件列表信息
    m_file_info_map->insert(std::make_pair(info.file_id, info));
}

void TransManager::init()
{
    // 检查是否已经初始化
    if (m_is_init)
    {
        DANEJOE_LOG_TRACE("default", "TransManager", "TransManager has been initialized");
        return;
    }
    // 初始化块信息服务
    m_block_request_info_service.init();
    // 更新文件信息处理线程标志位
    m_is_running.store(true);
    // 初始化文件信息队列
    m_client_file_info_queue = DaneJoe::MTQueue<ClientFileInfo>(128);
    // 初始化文件信息列表
    m_file_info_map = std::make_shared<std::unordered_map<int32_t, ClientFileInfo>>();
    // 添加默认处理线程
    add_thread();
    // 初始化文件信息处理线程
    m_client_file_info_thread = std::thread([&]()
        {
            // 文件信息处理主循环
            while (m_is_running.load())
            {
                // 弹出文件信息，进行检查
                auto client_file_info_option = m_client_file_info_queue.pop();
                if (!client_file_info_option.has_value())
                {
                    continue;
                }
                auto client_file_info = client_file_info_option.value();
                // 获取文件请求块列表
                auto block_info_list = m_block_request_info_service.get_by_file_id_and_state(client_file_info.file_id, FileState::Waiting);
                // 获取块请求任务队列
                auto m_block_task_queue_it = m_block_task_queue_list.begin();
                // 默认使用第一个队列
                /// @todo 添加平衡负载逻辑
                if (m_block_task_queue_it == m_block_task_queue_list.end())
                {
                    DANEJOE_LOG_ERROR("default", "TransManager", "No block task queue");
                    continue;
                }
                // 更新块请求队列
                for (auto& block_info : block_info_list)
                {
                    (*m_block_task_queue_it)->push(block_info);
                }
            }
        });
    // 更新初始化标记
    m_is_init = true;
}

void TransManager::add_thread()
{
    // 检查当前块请求线程数量是否超出限制
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
    block_request_thread->init(m_file_info_map);
    block_request_thread->start();
    m_thread_count++;
}
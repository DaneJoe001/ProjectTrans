#include "client/connect/block_request_thread.hpp"
#include "log/manage_logger.hpp"

void BlockRequestThread::init(std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> block_task_queue)
{
    m_block_task_queue = block_task_queue;
}

void BlockRequestThread::run()
{
    m_is_running.store(true);
    while (m_is_running.load())
    {
        if (!m_block_task_queue)
        {
            QThread::msleep(100);
            continue;
        }
        auto block_request_info_opt = m_block_task_queue->pop();
        if (!block_request_info_opt.has_value())
        {
            QThread::msleep(100);
            continue;
        }
        auto block_request_info = block_request_info_opt.value();
        DANEJOE_LOG_TRACE("default", "BlockRequestThread", "Reques block: {}", block_request_info.to_string());
        /// @todo 获取连接后开始请求
        /// @todo 完成请求并写入文件后发送完成信号，完成数据库更新
        emit block_request_finished(block_request_info.file_id, block_request_info.block_id);
    }
}

void BlockRequestThread::stop()
{
    m_is_running.store(false);
    if (m_block_task_queue)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestThread", "Stop queue");
        m_block_task_queue->close();
    }
}

void BlockRequestThread::on_file_state_changed(int32_t file_id, FileState state)
{
    m_file_state_map[file_id] = state;
}

BlockRequestThread::BlockRequestThread(QObject* parent) :QThread(parent)
{

}

BlockRequestThread::~BlockRequestThread()
{

}

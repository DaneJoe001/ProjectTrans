#include "client/connect/block_request_thread.hpp"
#include "log/manage_logger.hpp"
#include "client/connect/connection_manager.hpp"
#include "common/network/url_resolver.hpp"
#include "client/connect/message_handler.hpp"

void BlockRequestThread::init(std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> block_task_queue)
{
    // 检查块队列是否已经初始化，已初始化发出警告，完成替换
    if (m_block_task_queue)
    {
        DANEJOE_LOG_WARN("default", "BlockRequestThread", "m_block_task_queue already initialized");
    }
    m_block_task_queue = block_task_queue;
}

void BlockRequestThread::run()
{
    // 更新线程标志
    m_is_running.store(true);
    // 线程循环
    while (m_is_running.load())
    {
        // 检查块队列是否已经初始化
        if (!m_block_task_queue)
        {
            // 未初始化则延时等待
            /// @todo 考虑是否抽出为配置
            QThread::msleep(100);
            continue;
        }
        // 尝试从队列中获取一个块请求信息
        auto block_request_info_opt = m_block_task_queue->pop();
        // 检查是否有有效数据
        if (!block_request_info_opt.has_value())
        {
            // 无数据则延时等待，进入下一次循环
            QThread::msleep(100);
            continue;
        }
        // 获取块请求信息
        auto block_request_info = block_request_info_opt.value();
        DANEJOE_LOG_TRACE("default", "BlockRequestThread", "Reques block: {}", block_request_info.to_string());
        // 在文件信息表中查找是否有该文件信息记录
        auto m_file_info_it = m_file_info_map->find(block_request_info.file_id);
        if (m_file_info_it == m_file_info_map->end())
        {
            /// @todo 考虑是否在数据库中查找并更新文件信息
            continue;
        }
        // 从文件源路径中提取url信息
        UrlResolver::UrlInfo url_info = UrlResolver::parse(m_file_info_it->second.source_path);
        // 当前只处理自定义协议
        if (url_info.protocol != UrlResolver::UrlProtocol::Danejoe)
        {
            continue;
        }
        auto block_request_data = Client::MessageHandler::build_block_request(url_info,block_request_info);
        // 获取连接管理器
        auto& connection_manager = ConnectionManager::get_instance();
        // 添加连接
        connection_manager.add_connection(url_info.host, url_info.port);
        // 获取连接
        auto guard_connection = connection_manager.get_connection_guard(url_info.host, url_info.port);
        // 检查是否已连接
        if (!guard_connection->is_connected())
        {
            /// @todo 考虑是否重试
            continue;
        }
        // 发送数据
        guard_connection->send(block_request_data);

        // 接收数据响应
        auto response_data = guard_connection->receive();
        // 检查响应数据是否为空
        if (response_data.empty())
        {
            /// @todo 超时重试
            continue;
        }
        // 从接收的数据构建字符串
        Client::MessageHandler::parse_block_response(response_data);
        // 发送收到块的信号
        /// @todo 实现线程安全化，在对应槽检查是否完全接收，并验证下载完全
        /// @todo 在TransManager中添加一个槽，接收块信号，并调用相应的方法
        emit block_request_finished(block_request_info.file_id, block_request_info.block_id);
    }
}

void BlockRequestThread::stop()
{
    // 更新标志位，停止线程循环
    m_is_running.store(false);
    quit();
    wait();
}

void BlockRequestThread::on_file_state_changed(int32_t file_id, FileState state)
{
    m_file_state_map[file_id] = state;
}

void BlockRequestThread::init(std::shared_ptr<std::unordered_map<int32_t, ClientFileInfo>> file_info_map)
{
    m_file_info_map = file_info_map;
}

BlockRequestThread::BlockRequestThread(QObject* parent) :QThread(parent)
{

}

BlockRequestThread::~BlockRequestThread()
{
    stop();
}

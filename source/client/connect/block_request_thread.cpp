#include <fstream>

#include <danejoe/logger/logger_manager.hpp>
#include "danejoe/network/url/url_resolver.hpp"

#include "client/connect/block_request_thread.hpp"
#include "client/connect/connection_manager.hpp"
#include "client/connect/message_handler.hpp"

void BlockRequestThread::init(std::shared_ptr<DaneJoe::MpmcBoundedQueue<BlockRequestInfo>> block_task_queue)
{
    // 检查块队列是否已经初始化，已初始化发出警告，完成替换
    if (m_block_task_queue)
    {
        DANEJOE_LOG_WARN("default", "BlockRequestThread", "m_block_task_queue already initialized");
    }
    m_block_task_queue = block_task_queue;
    m_file_info_service.init();
    m_block_request_info_service.init();
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
        DaneJoe::UrlInfo url_info = DaneJoe::UrlResolver::parse(m_file_info_it->second.source_path);
        // 当前只处理自定义协议
        if (url_info.protocol != DaneJoe::UrlProtocol::Danejoe)
        {
            continue;
        }
        auto block_request_data = Client::MessageHandler::build_block_request(url_info, block_request_info);
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
        auto frame_opt = m_frame_assembler.pop_frame();
        do
        {
            auto response_data = guard_connection->receive();
            m_frame_assembler.push_data(response_data);
            frame_opt = m_frame_assembler.pop_frame();
        } while (!frame_opt.has_value());

        auto response_data = frame_opt.value();
        // 检查响应数据是否为空
        if (response_data.empty())
        {
            /// @todo 超时重试
            continue;
        }
        // 从接收的数据构建字符串
        auto info_opt = Client::MessageHandler::parse_response(response_data);
        if (!info_opt.has_value())
        {
            DANEJOE_LOG_WARN("default", "BlockRequestThread", "Failed to parse response data");
            continue;
        }
        auto info = info_opt.value();
        auto block_info_opt = Client::MessageHandler::parse_block_response(info_opt.value().body);
        if (!block_info_opt.has_value())
        {
            DANEJOE_LOG_WARN("default", "BlockRequestThread", "Failed to parse block response");
            continue;
        }
        auto block_info = block_info_opt.value();
        write_block_data(block_info);
        // 发送收到块的信号
        /// @todo 实现线程安全化，在对应槽检查是否完全接收，并验证下载完全
        /// @todo 在TransManager中添加一个槽，接收块信号，并调用相应的方法
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
    m_file_info_service.init();
    m_block_request_info_service.init();
}

void BlockRequestThread::write_block_data(const BlockResponseInfo& block_response_info)
{
    auto file_info_opt = m_file_info_service.get_by_id(block_response_info.file_id);
    if (!file_info_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "BlockRequestThread", "Failed to get file info");
        return;
    }
    auto file_info = file_info_opt.value();
    std::ofstream file(file_info.saved_path, std::ios::out | std::ios::binary | std::ios::app);
    file.seekp(block_response_info.offset, std::ios::beg);
    file.write(reinterpret_cast<const char*>(block_response_info.data.data()), block_response_info.data.size());
    file.close();
    auto block_request_info_opt = m_block_request_info_service.get_by_id(block_response_info.block_id);
    if (!block_request_info_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "BlockRequestThread", "Failed to get block request info");
        return;
    }
    auto block_request_info = block_request_info_opt.value();
    DANEJOE_LOG_TRACE("default", "BlockRequestThread", "Write block data: file_id = {}, block_id = {}, offset = {}, size = {}", block_response_info.file_id, block_response_info.block_id, block_response_info.offset, block_response_info.data.size());
    block_request_info.state = FileState::Completed;
    block_request_info.end_time = std::chrono::steady_clock::now();
    m_block_request_info_service.update(block_request_info);
    m_block_request_info_service.remove(block_response_info.block_id);
    emit update();
    emit block_request_finished(block_request_info.file_id, block_request_info.block_id);
}

BlockRequestThread::BlockRequestThread(QObject* parent) :QThread(parent)
{

}

BlockRequestThread::~BlockRequestThread()
{
    stop();
}

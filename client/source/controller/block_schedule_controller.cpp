#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>

#include "danejoe/logger/logger_manager.hpp"

#include "controller/block_schedule_controller.hpp"
#include "model/transfer/block_transfer.hpp"

using namespace std::literals;

BlockScheduleController::BlockScheduleController(
    BlockService& block_service,
    TaskService& task_service,
    QPointer<ViewEventHub> view_event_hub,
    QObject* parent) :
    QObject(parent),
    m_view_event_hub(view_event_hub),
    m_block_service(block_service),
    m_task_service(task_service)

{}

void BlockScheduleController::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to init: Has been inited");
        return;
    }
    m_block_request_timer = new QTimer(this);
    m_block_request_timer->start(m_block_request_interval_ms);
    connect(m_block_request_timer, &QTimer::timeout, this, &BlockScheduleController::on_block_request);
    m_is_init = true;
    connect(m_view_event_hub, &ViewEventHub::block_response, this, &BlockScheduleController::on_block_response);
}

void BlockScheduleController::on_task_enqueue(
    EventContext event_source,
    NetworkEndpoint endpoint,
    TaskEntity task_entity)
{
    auto blocks =
        m_block_service.get_by_task_id(task_entity.task_id);
    TaskPending task_pendding;
    task_pendding.endpoint = endpoint;
    task_pendding.event_source = event_source;
    task_pendding.task_entity = task_entity;
    task_pendding.is_paused = false;
    m_task_pendding_map[task_entity.task_id] = std::move(task_pendding);
    for (auto block : blocks)
    {
        BlockRequestTransfer transfer;
        transfer.block_id = block.block_id;
        transfer.file_id = block.file_id;
        transfer.task_id = block.task_id;
        transfer.offset = block.offset;
        transfer.block_size = block.block_size;
        m_block_request_transfer_queue.enqueue(transfer);
    }
}

void BlockScheduleController::on_task_cancle(int64_t task_id)
{
    auto task_schedule_it =
        m_task_pendding_map.find(task_id);
    if (task_schedule_it == m_task_pendding_map.end())
    {
        return;
    }
    m_task_pendding_map.erase(task_schedule_it);
}

void BlockScheduleController::on_task_paused(int64_t task_id, bool is_paused)
{
    auto task_schedule_it =
        m_task_pendding_map.find(task_id);
    if (task_schedule_it == m_task_pendding_map.end())
    {
        return;
    }
    m_task_pendding_map[task_id].is_paused = is_paused;
}

void BlockScheduleController::on_block_request()
{
    if (m_block_request_transfer_queue.empty())
    {
        return;
    }
    auto transfer =
        m_block_request_transfer_queue.dequeue();
    auto task_penddind_it = m_task_pendding_map.find(transfer.task_id);
    if (task_penddind_it == m_task_pendding_map.end())
    {
        return;
    }
    if (task_penddind_it->second.is_paused)
    {
        m_block_request_transfer_queue.enqueue(transfer);
        return;
    }
    m_view_event_hub->publish_block_request(
        task_penddind_it->second.event_source,
        task_penddind_it->second.endpoint, transfer);
}

void BlockScheduleController::on_block_response(
    EventEnvelope event_envelope,
    TransContext trans_context,
    BlockResponseTransfer response)
{
    DANEJOE_LOG_TRACE("default", "BlockScheduleController", "Source: {},TransContext: {}!", event_envelope.m_event_context.m_object_name, trans_context.request_id);
    auto task_pendding_it =
        m_task_pendding_map.find(response.task_id);
    if (task_pendding_it == m_task_pendding_map.end())
    {
        DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to find task!");
        return;
    }
    auto block_entity_opt =
        m_block_service.get_by_id(response.block_id);
    if (!block_entity_opt.has_value())
    {
        return;
    }
    auto block_entity = block_entity_opt.value();
    QFileInfo dest_file_info(QString::fromStdString(task_pendding_it->second.task_entity.saved_path));
    if (!task_pendding_it->second.dest_file)
    {
        task_pendding_it->second.dest_file = std::make_unique<QFile>(dest_file_info.absoluteFilePath());
    }
    if (!task_pendding_it->second.dest_file->isOpen())
    {
        QDir dir = dest_file_info.absoluteDir();
        if (!dir.exists())
        {
            auto is_mkdir = dir.mkpath(".");
            if (!is_mkdir)
            {
                DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to create dest directory");
                block_entity.state = BlockState::Failed;
                m_block_service.update(block_entity);
                return;
            }
        }
        auto is_open =
            task_pendding_it->second.dest_file->open(QIODevice::ReadWrite);
        if (!is_open)
        {
            DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to open file");
            block_entity.state = BlockState::Failed;
            m_block_service.update(block_entity);
            return;
        }
    }

    auto is_seeked = task_pendding_it->second.dest_file->seek(response.offset);
    if (!is_seeked)
    {
        DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to seek file");
        block_entity.state = BlockState::Failed;
        m_block_service.update(block_entity);
        return;
    }
    int64_t has_write = 0;
    if (static_cast<std::size_t>(response.block_size) > response.data.size())
    {
        DANEJOE_LOG_WARN("default", "BlockScheduleController", "No enough data");
        block_entity.state = BlockState::Failed;
        m_block_service.update(block_entity);
        return;
    }
    int64_t rest_bytes = response.block_size;

    while (rest_bytes > 0)
    {
        int64_t bytes_written =
            task_pendding_it->second.dest_file->write(reinterpret_cast<const char*>(response.data.data() + has_write), rest_bytes);
        if (bytes_written < 0)
        {
            DANEJOE_LOG_WARN("default", "BlockScheduleController", "Failed to write data");
            block_entity.state = BlockState::Failed;
            m_block_service.update(block_entity);
            return;
        }
        rest_bytes -= bytes_written;
        has_write += bytes_written;
    }
    task_pendding_it->second.dest_file->flush();
    block_entity.state = BlockState::Completed;
    block_entity.end_time = std::chrono::system_clock::now();
    m_block_service.update(block_entity);
    auto rest_block_count =
        m_block_service.get_count_by_task_id_and_block_state(task_pendding_it->second.task_entity.task_id, BlockState::Waiting);
    if (rest_block_count == 0)
    {
        auto task_entity_opt = m_task_service.get_by_task_id(task_pendding_it->second.task_entity.task_id);
        if (task_entity_opt.has_value())
        {
            auto task_entity = task_entity_opt.value();
            task_entity.end_time = std::chrono::system_clock::now();
            task_entity.state = TaskState::Completed;
            bool is_updated = m_task_service.update(task_entity);
            DANEJOE_LOG_DEBUG("default", "BlockScheduleController", "Updated:{}", is_updated);
        }
        /// @todo 后续再考虑其他处理
        emit task_completed(task_pendding_it->second.task_entity.task_id);
    }

}
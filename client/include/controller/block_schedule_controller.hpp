#pragma once

#include <memory>
#include <unordered_map>

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QTimer>
#include <QFile>
#include <QPointer>

#include "model/entity/task_entity.hpp"
#include "service/block_service.hpp"
#include "view/event/view_event_hub.hpp"

struct TaskPendding
{
    bool is_paused = false;
    EventContext event_source;
    NetworkEndpoint endpoint;
    TaskEntity task_entity;
    std::unique_ptr<QFile> dest_file;
};

class BlockScheduleController : public QObject
{
    Q_OBJECT
public:
    BlockScheduleController(
        BlockService& m_block_service,
        QPointer<ViewEventHub> view_event_hub,
        QObject* parent = nullptr);
    void init();
signals:
    void task_completed(int64_t task_id);
public slots:
    void on_task_enqueue(
        EventContext event_source,
        NetworkEndpoint endpoint,
        TaskEntity task_entity);
    void on_task_cancle(int64_t task_id);
    void on_task_paused(int64_t task_id, bool is_paused);
    void on_block_request();
    void on_block_response(
        EventEnvelope event_envelope,
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    bool m_is_init = false;
    QTimer* m_block_request_timer;
    QQueue<BlockRequestTransfer> m_block_request_transfer_queue;
    int m_block_request_interval_ms = 30;
    QPointer<ViewEventHub> m_view_event_hub = nullptr;
    BlockService& m_block_service;
    std::unordered_map<int64_t, TaskPendding> m_task_pendding_map;

};
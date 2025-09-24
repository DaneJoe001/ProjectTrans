#pragma once

#include <memory>
#include <atomic>
#include <unordered_map>

#include <QThread>

#include "mt_queue/mt_queue.hpp"
#include "client/model/block_request_info.hpp"

class BlockRequestThread : public QThread {
public:
    Q_OBJECT
public:
    BlockRequestThread(QObject* parent = nullptr);
    ~BlockRequestThread();
    void init(std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> block_task_queue);
    void run() override;
signals:
    void block_request_finished(int32_t file_id, int32_t block_id);
public slots:
    void stop();
    void on_file_state_changed(int32_t file_id, FileState state);
private:
    std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> m_block_task_queue = nullptr;
    std::atomic<bool> m_is_running = false;
    std::unordered_map<int32_t, FileState> m_file_state_map;
};
#pragma once

/**
 * @file block_request_thread.hpp
 * @brief 块请求线程
 * @author DaneJoe001
 */

#include <memory>
#include <atomic>
#include <unordered_map>

#include <QThread>

#include "mt_queue/mt_queue.hpp"
#include "client/model/block_request_info.hpp"
#include "client/model/client_file_info.hpp"

 /**
  * @brief 块请求线程，继承自QThread
  * @details 负责处理块请求的线程
  * @note 该线程负责处理块请求
  */
class BlockRequestThread : public QThread {
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    BlockRequestThread(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~BlockRequestThread();
    /**
     * @brief 初始化
     * @param block_task_queue 块请求队列
     */
    void init(std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> block_task_queue);
    /**
     * @brief 初始化
     * @param file_info_map 文件信息映射
     */
    void init(std::shared_ptr<std::unordered_map<int32_t, ClientFileInfo>> file_info_map);
    /**
     * @brief 运行
     */
    void run() override;
signals:
    /**
     * @brief 块请求完成信号
     * @param file_id 文件ID
     * @param block_id 块ID
     */
    void block_request_finished(int32_t file_id, int32_t block_id);
public slots:
    /**
     * @brief 停止
     */
    void stop();
    /**
     * @brief 文件状态改变槽函数,用于标识文件是否暂停请求
     * @param file_id 文件ID
     * @param state 文件状态
     */
    void on_file_state_changed(int32_t file_id, FileState state);
private:
    /// @brief 块请求队列,此结构与TransManager共享，由TransManager管理
    std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>> m_block_task_queue = nullptr;
    /// @brief 是否运行
    std::atomic<bool> m_is_running = false;
    /// @brief 文件状态映射
    std::unordered_map<int32_t, FileState> m_file_state_map;
    /// @brief 文件信息映射,此信息与TransManager共享，由TransManager管理
    std::shared_ptr<std::unordered_map<int32_t, ClientFileInfo>> m_file_info_map = nullptr;
};
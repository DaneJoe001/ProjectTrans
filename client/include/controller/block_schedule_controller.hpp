/**
  * @file block_schedule_controller.hpp
  * @brief 块调度控制器
  * @author DaneJoe001
  * @date 2026-01-06
  */
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
#include "service/task_service.hpp"
#include "view/event/view_event_hub.hpp"

  /**
   * @struct TaskPendding
   * @brief 任务调度中的暂存信息
   * @details 用于记录单个任务在块请求/响应调度过程中的状态与资源。
   */
struct TaskPending
{
    /// @brief 是否暂停调度
    bool is_paused = false;
    /// @brief 事件来源上下文（用于回传/关联 UI 触发源）
    EventContext event_source;
    /// @brief 目标网络端点
    NetworkEndpoint endpoint;
    /// @brief 任务实体数据
    TaskEntity task_entity;
    /// @brief 目标输出文件（下载写入）
    std::unique_ptr<QFile> dest_file;
};

/**
 * @class BlockScheduleController
 * @brief 块调度控制器
 * @details 负责管理任务队列与定时触发块请求，并在收到块响应后推进任务进度。
 */
class BlockScheduleController : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param m_block_service 块服务引用
     * @param view_event_hub 视图事件中心
     * @param parent Qt 父对象
     */
    BlockScheduleController(
        BlockService& m_block_service,
        TaskService& task_service,
        QPointer<ViewEventHub> view_event_hub,
        QObject* parent = nullptr);
    /**
     * @brief 初始化
     * @details 建立必要的信号槽连接并启动块请求定时器。
     */
    void init();
signals:
    /**
     * @brief 任务完成信号
     * @param task_id 任务ID
     */
    void task_completed(int64_t task_id);
public slots:
    /**
     * @brief 入队一个任务
     * @param event_source 事件来源上下文
     * @param endpoint 目标网络端点
     * @param task_entity 任务实体
     */
    void on_task_enqueue(
        EventContext event_source,
        NetworkEndpoint endpoint,
        TaskEntity task_entity);
    /**
     * @brief 取消任务
     * @param task_id 任务ID
     */
    void on_task_cancle(int64_t task_id);
    /**
     * @brief 暂停/恢复任务
     * @param task_id 任务ID
     * @param is_paused 是否暂停
     */
    void on_task_paused(int64_t task_id, bool is_paused);
    /**
     * @brief 定时触发块请求
     */
    void on_block_request();
    /**
     * @brief 处理块响应
     * @param event_envelope 事件信封
     * @param trans_context 传输上下文
     * @param response 块响应
     */
    void on_block_response(
        EventEnvelope event_envelope,
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 块请求定时器
    QTimer* m_block_request_timer;
    /// @brief 待发送的块请求队列
    QQueue<BlockRequestTransfer> m_block_request_transfer_queue;
    /// @brief 块请求间隔（毫秒）
    int m_block_request_interval_ms = 30;
    /// @brief 视图事件中心
    QPointer<ViewEventHub> m_view_event_hub = nullptr;
    /// @brief 块服务引用
    BlockService& m_block_service;
    TaskService& m_task_service;
    /// @brief 任务暂存表（按 task_id 索引）
    std::unordered_map<int64_t, TaskPending> m_task_pendding_map;

};
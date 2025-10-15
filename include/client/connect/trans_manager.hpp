#pragma once

/**
 * @file trans_manager.hpp
 * @brief 传输管理器
 * @author DaneJoe001
 */

#include <unordered_map>
#include <cstdint>
#include <thread>

#include <QList>
#include <QObject>

#include "mt_queue/mt_queue.hpp"
#include "client/model/trans_info.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"
#include "client/connect/block_request_thread.hpp"
#include "client/service/block_request_info_service.hpp"

class ConnectionManager;

/**
 * @class TransManager
 * @brief 传输管理器
 */
class TransManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
    */
    TransManager(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~TransManager();
    /**
     * @brief 新建传输完成时添加传输信息
     */
    void add_trans(const ClientFileInfo& info);
    /**
     * @brief 添加线程
     */
    void add_thread();
    /**
     * @brief 初始化
     */
    void init();
public slots:
private:
    /// @brief 是否运行
    std::atomic<bool> m_is_running = false;
    /// @brief 客户端文件信息线程，用于处理文件，完成块请求入队
    std::thread m_client_file_info_thread;
    /// @brief 客户端文件信息队列，用于客户端传输文件列表
    DaneJoe::MTQueue<ClientFileInfo>m_client_file_info_queue;
    /// @brief 块请求队列列表，用于存储块请求队列
    QList<std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>>>  m_block_task_queue_list;
    /// @brief 块请求线程列表，用于处理块请求线程
    QList<BlockRequestThread*> m_block_thread_list;
    /// @brief 传输信息映射，用于存储传输信息
    std::unordered_map<int32_t, TransInfo> m_trans_map;
    /// @brief 连接管理器
    ConnectionManager& m_connection_manager;
    /// @brief 最大线程数
    uint32_t m_max_thread_count = 1;
    /// @brief 当前线程数
    uint32_t m_thread_count = 0;
    /// @brief 是否初始化
    bool m_is_init = false;
    /// @brief 块请求信息服务
    BlockRequestInfoService m_block_request_info_service;
    /// @brief 文件信息映射，便于块请求获取url
    std::shared_ptr<std::unordered_map<int32_t, ClientFileInfo>> m_file_info_map = nullptr;
};
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

#include "client/model/trans_info.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"
#include "mt_queue/mt_queue.hpp"
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
    void add_thread();
    void init();
private:
    std::atomic<bool> m_is_running = false;
    std::thread m_client_file_info_thread;
    DaneJoe::MTQueue<ClientFileInfo>m_client_file_info_queue;
    QList<std::shared_ptr<DaneJoe::MTQueue<BlockRequestInfo>>>  m_block_task_queue_list;
    QList<BlockRequestThread*> m_block_thread_list;
    /// @brief 传输信息映射
    std::unordered_map<int32_t, TransInfo> m_trans_map;
    /// @brief 连接管理器
    ConnectionManager& m_connection_manager;
    uint32_t m_max_thread_count = 1;
    uint32_t m_thread_count = 0;
    bool m_is_init = false;
    BlockRequestInfoService m_block_request_info_service;
};
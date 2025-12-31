/**
 * @file client_app.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 客户端应用程序主类
 * @date 2025-12-28
 */
#pragma once

#include <QObject>
#include <QThread>
#include <QPointer>

#include "service/trans_service.hpp"
#include "service/block_service.hpp"
#include "service/client_file_service.hpp"
#include "service/task_service.hpp"
#include "controller/view_event_controller.hpp"
#include "controller/block_schedule_controller.hpp"
#include "view/event/view_event_hub.hpp"


class ClientMainWindow;
class ViewEventController;

/**
 * @class ClientApp
 * @brief 客户端应用程序主类
 * @details 负责客户端应用程序的初始化、数据库管理、日志管理和主窗口显示
 */
class ClientApp : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为 nullptr
     */
    ClientApp(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     * @details 清理主窗口资源
     */
    ~ClientApp();
    /**
     * @brief 初始化应用程序
     * @details 初始化日志系统、数据库、事件中心、事件控制器和主窗口
     */
    void init();
    /**
     * @brief 显示主窗口
     * @details 如果应用程序已初始化，则显示客户端主窗口
     */
    void show_main_window();
signals:
    void stop_app();
private:
    /**
     * @brief 初始化数据库
     * @details 创建并配置客户端数据库，初始化传输服务、文件服务和块服务
     */
    void init_database();
    /**
     * @brief 清理数据库
     * @details 删除客户端数据库文件
     */
    void clear_database();
    /**
     * @brief 初始化日志系统
     * @details 配置日志输出路径和格式设置
     */
    void init_logger();
    /**
     * @brief 清理日志文件
     * @details 删除客户端日志文件
     */
    void clear_log();

private:
    /// @brief 初始化标志，标识应用程序是否已初始化
    bool m_is_init = false;
    /// @brief 视图事件中心指针，用于事件发布和订阅
    QPointer<ViewEventHub> m_view_event_hub;
    /// @brief 客户端主窗口指针
    ClientMainWindow* m_main_window = nullptr;
    /// @brief 视图事件控制器指针，处理视图层事件
    ViewEventController* m_view_event_controller = nullptr;
    QPointer<BlockScheduleController> m_block_schedule_controller = nullptr;
    QThread* m_block_schedule_thread = nullptr;
    /// @brief 传输服务引用，处理网络传输
    TransService m_trans_service;
    TaskService m_task_service;
    /// @brief 块服务引用，处理数据块操作
    BlockService m_block_service;
    /// @brief 客户端文件服务引用，处理文件操作
    ClientFileService m_client_file_service;
};
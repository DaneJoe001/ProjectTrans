/**
 * @file client_main_window.hpp
 * @brief 客户端主窗口
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <cstdint>

#include <QMainWindow>
#include <QString>
#include <QPointer>

#include "danejoe/network/url/url_resolver.hpp"

#include "view/event/view_event_hub.hpp"

#include "controller/block_schedule_controller.hpp"

#include "service/client_file_service.hpp"
#include "service/task_service.hpp"
#include "service/block_service.hpp"

class QMenu;
class QAction;
class QMenuBar;
class QStackedWidget;
class ConnectionTestDialog;
class NewDownloadDialog;
class NewUploadDialog;
class TaskTableWidget;
class TaskTableModel;

/**
 * @class ClientMainWindow
 * @brief 客户端主窗口
 * @details 客户端主界面容器：管理菜单与对话框，展示任务列表，并通过 ViewEventHub/BlockScheduleController
 *          等组件触发任务相关事件。
 */
class ClientMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param task_service 任务服务
     * @param client_file_service 客户端文件服务
     * @param block_service 块服务
     * @param view_event_hub 视图事件中心
     * @param block_schedule_controller 块调度控制器
     * @param parent 父窗口
     */
    ClientMainWindow(
        TaskService& task_service,
        ClientFileService& client_file_service,
        BlockService& block_service,
        QPointer<ViewEventHub> view_event_hub,
        QPointer<BlockScheduleController> block_schedule_controller,
        QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~ClientMainWindow();
    /**
     * @brief 初始化
     */
    void init();
  signals:
    /**
     * @brief 任务入队信号
     * @param event_context 事件上下文
     * @param endpoint 目标网络端点
     * @param task_entity 任务实体
     */
    void task_enqueue(
        EventContext event_context,
        NetworkEndpoint endpoint,
        TaskEntity task_entity);
public slots:
    /**
     * @brief 连接测试动作触发
     */
    void on_connection_test_action_triggered();
    /**
     * @brief 连接测试对话框关闭
     */
    void on_connection_test_dialog_closed();
    /**
     * @brief 新上传动作触发
     */
    void on_new_upload_action_triggered();
    /**
     * @brief 新下载动作触发
     */
    void on_new_download_action_triggered();
    /**
     * @brief 文件传输选中
     * @param task_id 任务ID
     */
    void on_task_selected(int64_t task_id);
    /**
     * @brief 开始任务动作触发
     */
    void on_start_task_action_triggered();
    /**
     * @brief 停止任务动作触发
     */
    void on_stop_task_action_triggered();
    /**
     * @brief 视图更新
     */
    void on_view_update();
    /**
     * @brief 任务完成
     * @param task_id 任务ID
     */
    void on_task_completed(int64_t task_id);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief URL解析器
    DaneJoe::UrlResolver m_resolver;
    /// @brief 任务服务
    TaskService& m_task_service;
    /// @brief 客户端文件服务
    ClientFileService& m_client_file_service;
    /// @brief 块服务
    BlockService& m_block_service;
    /// @brief 窗口标题
    QString m_window_title = "Efficient Transfer Client";
    /// @brief 菜单栏
    QMenuBar* m_menubar = nullptr;
    /// @brief 任务菜单
    QMenu* m_task_menu = nullptr;
    /// @brief 开始任务动作
    QAction* m_start_task_action = nullptr;
    /// @brief 停止任务动作
    QAction* m_stop_task_action = nullptr;
    /// @brief 设置动作
    QAction* m_setting_action = nullptr;
    /// @brief 连接测试动作
    QAction* m_connection_test_action = nullptr;
    /// @brief 下载菜单
    QMenu* m_download_menu = nullptr;
    /// @brief 新下载动作
    QAction* m_new_download_action = nullptr;
    /// @brief 上传菜单
    QMenu* m_upload_menu = nullptr;
    /// @brief 新上传动作
    QAction* m_new_upload_action = nullptr;
    /// @brief 视图菜单
    QMenu* m_view_menu = nullptr;
    /// @brief 排序动作
    QAction* m_sort_action = nullptr;
    /// @brief 连接测试对话框
    ConnectionTestDialog* m_connection_test_dialog = nullptr;
    /// @brief 新下载对话框
    NewDownloadDialog* m_new_download_dialog = nullptr;
    /// @brief 新上传对话框
    NewUploadDialog* m_new_upload_dialog = nullptr;
    /// @brief 堆叠窗口
    QStackedWidget* m_stack_widget = nullptr;
    /// @brief 文件传输信息窗口
    TaskTableWidget* m_task_table_widget = nullptr;
    /// @brief 视图事件中心
    QPointer<ViewEventHub> m_view_event_hub = nullptr;
    /// @brief 任务列表 TableModel
    QPointer<TaskTableModel> m_task_table_model = nullptr;
    /// @brief 块调度控制器
    QPointer<BlockScheduleController> m_block_schedule_controller = nullptr;
    /// @brief 当前选中的任务ID
    int64_t m_selected_task_id = -1;
};
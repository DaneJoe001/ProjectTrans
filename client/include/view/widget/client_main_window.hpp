/**
 * @file client_main_window.hpp
 * @brief 客户端主窗口
 * @author DaneJoe001
 */
#pragma once

#include <cstdint>

#include <QMainWindow>
#include <QString>
#include <QPointer>

#include "view/event/view_event_hub.hpp"

class QMenu;
class QAction;
class QMenuBar;
class QStackedWidget;
class ConnectionTestDialog;
class NewDownloadDialog;
class NewUploadDialog;
class TaskTableWidget;

/**
 * @class ClientMainWindow
 * @brief 客户端主窗口
 */
class ClientMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    ClientMainWindow(
        QPointer<ViewEventHub> view_event_hub,
        QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~ClientMainWindow();
    void init();
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
     * @param row 行
     */
    void on_file_trans_selected(int32_t row);
    /**
     * @brief 开始任务动作触发
     */
    void on_start_task_action_triggered();
    /**
     * @brief 停止任务动作触发
     */
    void on_stop_task_action_triggered();
    void on_view_update();
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
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
    QPointer<ViewEventHub> m_view_event_hub;
};
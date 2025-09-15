#pragma once

/**
 * @file client_main_window.hpp
 * @brief 客户端主窗口
 * @author DaneJoe001
 */

#include <cstdint>
#include <vector>

#include <QMainWindow>
#include <QString>


class QMenuBar;
class QMenu;
class QAction;
class QMenuBar;
class QStackedWidget;
class ConnectionTestDialog;
class TransDialog;
class NewDownloadDialog;
class NewUploadDialog;
class FileTransInfoWidget;

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
    ClientMainWindow(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~ClientMainWindow();
    void init();
signals:
    /**
     * @brief 数据接收完成信号，定时器循环调用，接收数据
     * @param data 接收到的数据
     */
    void message_received(const std::vector<uint8_t>& data);
private:
    /**
     * @brief 定时器事件
     * @param event 定时器事件
     */
    void timerEvent(QTimerEvent* event)override;
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
private:
    /// @brief 是否在连接测试
    bool m_is_on_connection_test = false;
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
    /// @brief 传输对话框
    TransDialog* m_trans_dialog = nullptr;
    /// @brief 新下载对话框
    NewDownloadDialog* m_new_download_dialog = nullptr;
    /// @brief 新上传对话框
    NewUploadDialog* m_new_upload_dialog = nullptr;
    /// @brief 堆叠窗口
    QStackedWidget* m_stack_widget = nullptr;
    /// @brief 文件传输信息窗口
    FileTransInfoWidget* m_file_trans_info_widget = nullptr;
};
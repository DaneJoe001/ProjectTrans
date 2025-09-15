#pragma once

/**
 * @file server_main_window.hpp
 * @brief 服务器主窗口
 * @author DaneJoe001
 */

#include <QMainWindow>

class QStackedWidget;
class QMenuBar;
class QMenu;
class QAction;
class AddFileDialog;
class ResourceInfoWidget;
class ConnectionInfoWidget;

/**
 * @class ServerMainWindow
 * @brief 服务器主窗口
 */
class ServerMainWindow : public QMainWindow {
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    ServerMainWindow(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 关闭事件
     * @param event 事件
     */
    void closeEvent(QCloseEvent* event) override;
    /**
     * @brief 定时器事件
     * @param event 事件
     */
    void timerEvent(QTimerEvent* event) override;
public slots:
    /**
     * @brief 添加文件动作触发
     */
    void on_add_file_action_triggered();
    /**
     * @brief 删除文件动作触发
     */
    void on_remove_file_action_triggered();
    /**
     * @brief 资源信息视图动作触发
     */
    void on_resource_info_view_action_triggered();
    /**
     * @brief 连接信息视图动作触发
     */
    void on_connection_info_view_action_triggered();
private:
    /// @brief 堆叠小部件
    QStackedWidget* m_stacked_widget = nullptr;
    /// @brief 菜单栏
    QMenuBar* m_menu_bar = nullptr;
    /// @brief 文件菜单
    QMenu* m_file_menu = nullptr;
    /// @brief 添加文件动作
    QAction* m_add_file_action = nullptr;
    /// @brief 删除文件动作
    QAction* m_remove_file_action = nullptr;
    /// @brief 视图菜单
    QMenu* m_view_menu = nullptr;
    /// @brief 资源信息视图动作
    QAction* m_resource_info_view_action = nullptr;
    /// @brief 连接信息视图动作
    QAction* m_connection_info_view_action = nullptr;
    /// @brief 添加文件对话框
    AddFileDialog* m_add_file_dialog = nullptr;

    /// @brief 资源信息小部件
    ResourceInfoWidget* m_resource_info_widget = nullptr;
    /// @brief 连接信息小部件
    ConnectionInfoWidget* m_connection_info_widget = nullptr;
};
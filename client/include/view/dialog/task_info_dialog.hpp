/**
 * @file task_info_dialog.hpp
 * @brief 任务信息对话框
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <QDialog>
#include <QPointer>
#include <QPushButton>

#include "view/event/view_event_hub.hpp"

class QPushButton;
class QTextBrowser;
class QStackedWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QFileDialog;

/**
 * @class TaskInfoDialog
 * @brief 任务信息对话框
 * @details 用于展示任务相关信息，并在等待/信息等不同面板间切换。
 */
class TaskInfoDialog : public QDialog
{
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param view_event_hub 视图事件中心
     * @param parent 父窗口
     */
    TaskInfoDialog(QPointer<ViewEventHub> view_event_hub, QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 切换到等待面板
     */
    void switch_to_waiting_panel();
    /**
     * @brief 切换到信息面板
     */
    void switch_to_info_panel();
    /**
     * @brief 处理下载响应
     * @param event_envelope 事件封包
     * @param trans_context 传输上下文
     * @param response 下载响应
     */
    void on_download_response(
        EventEnvelope event_envelope, TransContext trans_context,
        DownloadResponseTransfer response);

private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 主布局
    QVBoxLayout* m_main_layout = nullptr;
    /// @brief 堆栈小部件
    QStackedWidget* m_stack_widget = nullptr;

    /// @brief 信息小部件
    QWidget* m_info_widget = nullptr;
    /// @brief 信息布局
    QVBoxLayout* m_info_layout = nullptr;
    /// @brief 信息确认按钮
    QPushButton* m_info_ok_button = nullptr;
    /// @brief 信息取消按钮
    QPushButton* m_info_cancel_button = nullptr;
    /// @brief 信息文本浏览器
    QTextBrowser* m_info_text_browser = nullptr;
    /// @brief 信息按钮小部件
    QWidget* m_info_button_widget = nullptr;
    /// @brief 信息按钮布局
    QHBoxLayout* m_info_button_layout = nullptr;
    /// @brief 信息保存路径相关控件
    
    /// @brief 信息保存路径标签
    QLabel* m_info_saved_path_label = nullptr;
    /// @brief 信息保存路径编辑框
    QLineEdit* m_info_saved_path_line_edit = nullptr;
    /// @brief 信息保存路径按钮
    QPushButton* m_info_saved_path_button = nullptr;
    /// @brief 信息保存路径小部件
    QWidget* m_info_saved_path_widget = nullptr;
    /// @brief 信息保存路径布局
    QHBoxLayout* m_info_saved_path_layout = nullptr;

    /// @brief 等待部件窗口
    QWidget* m_waiting_widget = nullptr;
    /// @brief 等待布局
    QVBoxLayout* m_waiting_layout = nullptr;
    /// @brief 等待文本标签
    QLabel* m_waiting_text_label = nullptr;
    /// @brief 等待图标标签
    QLabel* m_waiting_icon_label = nullptr;
    /// @brief 等待取消按钮
    QPushButton* m_waiting_cancel_button = nullptr;

    /// @brief 错误部件窗口
    QWidget* m_error_widget = nullptr;
    /// @brief 错误布局
    QVBoxLayout* m_error_layout = nullptr;
    /// @brief 视图事件中心
    QPointer<ViewEventHub> m_view_event_hub;
};
/**
 * @file file_info_dialog.hpp
 * @brief 文件信息对话框
 * @author DaneJoe001
 */
#pragma once

#include <QDialog>
#include <QPointer>
#include <QPushButton>

#include "service/task_view_service.hpp"

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
 */
class TaskInfoDialog : public QDialog
{
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    TaskInfoDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 设置任务视图模型
     * @param task_view_model 任务视图模型
     */
    void set_task_view_model(const TaskViewModel& task_view_model);
    QPointer<QPushButton> get_waiting_cancel_button();
    QPointer<QPushButton> get_info_ok_button();
    QPointer<QPushButton> get_info_cancel_button();
    QPointer<QPushButton> get_info_saved_path_button();
private:
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
    /// @brief 信息保存路径标签

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
    /// @brief 是否已初始化
    bool m_is_init = false;
};
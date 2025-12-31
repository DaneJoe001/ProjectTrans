/**
 * @file new_download_dialog.hpp
 * @brief 新下载对话框
 * @author DaneJoe001
 */
#pragma once

#include <QDialog>
#include <QPointer>
#include <QPushButton>
#include <QMessageBox>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>
#include <qmessagebox.h>
#include "danejoe/network/url/url_resolver.hpp"

#include "service/block_service.hpp"
#include "view/event/view_event_hub.hpp"
#include "service/task_service.hpp"
#include "service/client_file_service.hpp"

class QPushButton;
class QLineEdit;
class QLabel;
class QTextBrowser;
class QProgressBar;
class QHBoxLayout;
class QVBoxLayout;
class TaskInfoDialog;

/**
 * @class NewDownloadDialog
 * @brief 新下载对话框
 */
class NewDownloadDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    NewDownloadDialog(
        TaskService& task_service,
        ClientFileService& client_file_service,
        BlockService& block_service,
        QPointer<ViewEventHub> view_event_hub,
        QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~NewDownloadDialog();
    /**
     * @brief 初始化
     */
    void init();
public slots:
    void on_add_download_push_button_clicked();
    void on_ok_to_add_file_button_clicked();
    void on_path_button_clicked();
    void on_download_response(
        EventEnvelope event_envelope, TransContext trans_context,
        DownloadResponseTransfer response);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    QPointer<ViewEventHub> m_view_event_hub;
    DaneJoe::UrlResolver m_url_resolver;
    TaskService& m_task_service;
    ClientFileService& m_client_file_service;
    BlockService& m_block_service;

    /// @brief 主布局
    QVBoxLayout* m_main_layout = nullptr;

    /// @brief URL标签
    QLabel* m_url_label = nullptr;
    /// @brief URL输入框
    QLineEdit* m_url_line_edit = nullptr;
    /// @brief 下载按钮
    QPushButton* m_add_download_push_button = nullptr;
    /// @brief URL布局
    QHBoxLayout* m_url_layout = nullptr;
    /// @brief URL窗口
    QWidget* m_url_widget = nullptr;

    /// @brief 用户名标签
    QLabel* m_username_label = nullptr;
    /// @brief 用户名输入框
    QLineEdit* m_username_line_edit = nullptr;
    /// @brief 密码标签
    QLabel* m_password_label = nullptr;
    /// @brief 密码输入框
    QLineEdit* m_password_line_edit = nullptr;
    /// @brief 用户信息布局
    QHBoxLayout* m_user_info_layout = nullptr;
    /// @brief 用户信息窗口
    QWidget* m_user_info_widget = nullptr;

    /// @brief 信息保存路径标签
    QLabel* m_saved_path_label = nullptr;
    /// @brief 信息保存路径编辑框
    QLineEdit* m_saved_path_line_edit = nullptr;
    /// @brief 信息保存路径按钮
    QPushButton* m_saved_path_button = nullptr;
    /// @brief 信息保存路径小部件
    QWidget* m_saved_path_widget = nullptr;
    /// @brief 信息保存路径布局
    QHBoxLayout* m_saved_path_layout = nullptr;

    /// @brief 下载信息浏览器
    QTextBrowser* m_download_info_browser = nullptr;
    /// @brief 文件信息对话框
    TaskInfoDialog* m_task_info_dialog = nullptr;

    QMessageBox* m_message_box = nullptr;
    QMessageBox* m_path_check_message_box = nullptr;

};
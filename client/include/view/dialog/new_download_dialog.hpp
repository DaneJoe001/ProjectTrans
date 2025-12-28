/**
 * @file new_download_dialog.hpp
 * @brief 新下载对话框
 * @author DaneJoe001
 */
#pragma once

#include <QDialog>
#include <QPointer>
#include <QPushButton>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

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
    NewDownloadDialog(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~NewDownloadDialog();
    /**
     * @brief 初始化
     */
    void init();
    QPointer<QPushButton> get_add_download_push_button();
    QPointer<QPushButton> get_ok_to_add_file_button();
private:
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
    /// @brief 下载进度条
    QProgressBar* m_download_progress_bar = nullptr;
    /// @brief 下载信息浏览器
    QTextBrowser* m_download_info_browser = nullptr;
    /// @brief 文件信息对话框
    TaskInfoDialog* m_file_info_dialog = nullptr;
    /// @brief 是否已初始化
    bool m_is_init = false;
};
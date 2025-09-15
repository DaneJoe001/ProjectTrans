#pragma once

/**
 * @file new_upload_dialog.hpp
 * @brief 新上传对话框
 * @author DaneJoe001
 */

#include <QDialog>

class QPushButton;
class QLineEdit;
class QLabel;
class QTextBrowser;
class QProgressBar;
class QHBoxLayout;
class QVBoxLayout;
class QFileDialog;

/**
 * @class NewUploadDialog
 * @brief 新上传对话框
 */
class NewUploadDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    NewUploadDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 主布局
    QVBoxLayout* m_main_layout = nullptr;
    /// @brief 文件对话框
    QFileDialog* m_file_dialog = nullptr;

    /// @brief 文件路径窗口
    QWidget* m_file_path_widget = nullptr;
    /// @brief 文件路径布局
    QHBoxLayout* m_file_path_layout = nullptr;
    /// @brief 文件路径标签
    QLabel* m_file_path_label = nullptr;
    /// @brief 文件路径输入框
    QLineEdit* m_file_path_line_edit = nullptr;
    /// @brief 文件路径选择按钮
    QPushButton* m_path_select_push_button = nullptr;
    /// @brief 添加文件按钮
    QPushButton* m_add_file_push_button = nullptr;

    /// @brief URL标签
    QLabel* m_url_label = nullptr;
    /// @brief URL输入框
    QLineEdit* m_url_line_edit = nullptr;
    /// @brief 上传按钮
    QPushButton* m_upload_push_button = nullptr;
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

    /// @brief 上传进度条
    QProgressBar* m_upload_progress_bar = nullptr;
    /// @brief 上传信息浏览器
    QTextBrowser* m_upload_info_browser = nullptr;
};
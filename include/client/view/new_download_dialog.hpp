#pragma once

/**
 * @file new_download_dialog.hpp
 * @brief 新下载对话框
 * @author DaneJoe001
 */
#include <thread>
#include <functional>
#include <atomic>

#include <QDialog>

#include "client/model/client_file_info.hpp"
#include "client/service/client_file_info_service.hpp"
#include "client/service/block_request_info_service.hpp"
#include "common/mt_queue/mt_queue.hpp"

class QPushButton;
class QLineEdit;
class QLabel;
class QTextBrowser;
class QProgressBar;
class QHBoxLayout;
class QVBoxLayout;
class ConnectionThread;
class FileInfoDialog;

/**
 * @struct TransFileInfo
 * @brief 文件传输信息，包括文件信息，传输配置
 */
struct TransFileInfo
{
    /// @brief 文件信息
    ClientFileInfo file_info;
    /// @brief 块参数配置
    BlockParamConfig block_param_config;
};

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
public slots:
    /**
     * @brief 下载按钮点击
     */
    void on_download_push_button_clicked();
    /**
     * @brief 接收原始文件信息信号
     * @param raw_file_info 原始文件信息
     * @param file_info 文件信息
     */
    void ok_to_add_file_info(ClientFileInfo file_info);
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
    /// @brief 连接线程
    ConnectionThread* m_connection_thread = nullptr;
    /// @brief 文件信息对话框
    FileInfoDialog* m_file_info_dialog = nullptr;
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 文件信息服务
    ClientFileInfoService m_file_info_service;
    /// @brief 块请求信息服务
    BlockRequestInfoService m_block_request_info_service;
    /// @brief 文件传输信息处理线程
    std::thread m_handle_trans_file_info_thread;
    /// @brief 是否已处理文件信息线程运行
    std::atomic<bool> m_is_handle_trans_file_info_thread_running = false;
    /// @brief 文件传输信息队列
    DaneJoe::MTQueue<TransFileInfo> m_handle_trans_file_info_queue;
};
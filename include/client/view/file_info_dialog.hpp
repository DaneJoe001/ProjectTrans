#pragma once

/**
 * @file file_info_dialog.hpp
 * @brief 文件信息对话框
 * @author DaneJoe001
 */

#include <vector>
#include <cstdint>

#include <QDialog>

#include "client/model/client_file_info.hpp"

class QPushButton;
class QTextBrowser;
class QStackedWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QFileDialog;

/**
 * @class FileInfoDialog
 * @brief 文件信息对话框
 */
class FileInfoDialog : public QDialog
{
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    FileInfoDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 设置操作
     * @param operation 操作
     */
    void set_operation(Operation operation);
signals:
    /**
     * @brief 接收原始文件信息信号
     * @param info 原始文件信息
     */
    void info_ok_button_clicked(ClientFileInfo file_info);
public slots:
    /**
     * @brief 接收原始文件信息槽函数
     * @param info 原始文件信息
     */
    void on_received_raw_file_info(std::vector<uint8_t> info);
    /**
     * @brief 等待取消按钮点击槽函数
     */
    void on_waiting_cancel_button_clicked();
    /**
     * @brief 点击信息确认按钮,确认代表确认下载，信息分块入库
     */
    void on_info_ok_button_clicked();
    /**
     * @brief 信息取消按钮点击槽函数
     */
    void on_info_cancel_button_clicked();
    /**
     * @brief 信息保存路径按钮点击槽函数
     */
    void on_info_saved_path_button_clicked();
private:
    /// @brief 原始文件信息
    ClientFileInfo m_info_received;

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

    /// @brief 操作
    Operation m_operation = Operation::Unknown;
};
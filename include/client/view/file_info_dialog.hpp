#pragma once

/**
 * @file file_info_dialog.hpp
 * @brief 文件信息对话框
 * @author DaneJoe001
 */

#include <vector>
#include <cstdint>
#include <string>

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
    void set_operation(Operation operation);
signals:
    void info_ok_button_clicked(std::string raw_file_info, ClientFileInfo file_info);
public slots:
    void on_received_raw_file_info(std::vector<uint8_t> info);
    void on_waiting_cancel_button_clicked();
    /**
     * @brief 点击信息确认按钮,确认代表确认下载，信息分块入库
     */
    void on_info_ok_button_clicked();
    void on_info_cancel_button_clicked();
    void on_info_saved_path_button_clicked();
private:
    std::string m_raw_file_info;

    QVBoxLayout* m_main_layout = nullptr;
    QStackedWidget* m_stack_widget = nullptr;

    QWidget* m_info_widget = nullptr;
    QVBoxLayout* m_info_layout = nullptr;
    QPushButton* m_info_ok_button = nullptr;
    QPushButton* m_info_cancel_button = nullptr;
    QTextBrowser* m_info_text_browser = nullptr;
    QWidget* m_info_button_widget = nullptr;
    QHBoxLayout* m_info_button_layout = nullptr;

    QLabel* m_info_saved_path_label = nullptr;
    QLineEdit* m_info_saved_path_line_edit = nullptr;
    QPushButton* m_info_saved_path_button = nullptr;
    QWidget* m_info_saved_path_widget = nullptr;
    QHBoxLayout* m_info_saved_path_layout = nullptr;

    QWidget* m_waiting_widget = nullptr;
    QVBoxLayout* m_waiting_layout = nullptr;
    QLabel* m_waiting_text_label = nullptr;
    QLabel* m_waiting_icon_label = nullptr;
    QPushButton* m_waiting_cancel_button = nullptr;

    QWidget* m_error_widget = nullptr;
    QVBoxLayout* m_error_layout = nullptr;
    /// @brief 是否已初始化
    bool m_is_init = false;

    Operation m_operation = Operation::Unknown;
};
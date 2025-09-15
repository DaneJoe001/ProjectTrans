#pragma once

/**
 * @file connection_test_dialog.hpp
 * @brief 连接测试对话框
 * @author DaneJoe001
 */

#include <QDialog>
#include <memory>

#include "client/connect/client_connection.hpp"

class QLineEdit;
class QPushButton;
class QLabel;
class QTextBrowser;
class QTextEdit;
class QHBoxLayout;
class QWidget;
class QVBoxLayout;
class ConnectionThread;

/**
 * @class ConnectionTestDialog
 * @brief 连接测试对话框
 */
class ConnectionTestDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    ConnectionTestDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 定时器事件
     * @param event 定时器事件
     */
    void timerEvent(QTimerEvent* event) override;
    /**
     * @brief 关闭事件
     * @param event 关闭事件
     */
    void closeEvent(QCloseEvent* event) override;
    /**
     * @brief 发送数据信号
     * @param data 数据
     */
signals:
    void send_data_signal(const std::vector<uint8_t>& data);
public slots:
    /**
     * @brief 发送按钮点击
     */
    void on_send_push_button_clicked();
    /**
     * @brief 连接按钮点击
     */
    void on_connect_push_button_clicked();
    /**
     * @brief 消息接收
     * @param data 数据
     */
    void on_message_received(const std::vector<uint8_t>& data);
private:
    /// @brief URL标签
    QLabel* m_url_label = nullptr;
    /// @brief 发送标签
    QLabel* m_send_label = nullptr;
    /// @brief 接收标签
    QLabel* m_recv_label = nullptr;
    /// @brief URL输入框
    QLineEdit* m_url_line_edit = nullptr;
    /// @brief 发送按钮
    QPushButton* m_send_push_button = nullptr;
    /// @brief 连接按钮
    QPushButton* m_connect_push_button = nullptr;
    /// @brief 发送文本编辑框
    QTextEdit* m_send_text_edit = nullptr;
    /// @brief 接收文本浏览器
    QTextBrowser* m_recv_text_browser = nullptr;
    /// @brief URL布局
    QHBoxLayout* m_url_layout = nullptr;
    /// @brief URL窗口
    QWidget* m_url_widget = nullptr;
    /// @brief 主布局
    QVBoxLayout* m_main_layout = nullptr;
    /// @brief 连接线程
    ConnectionThread* m_connection_thread = nullptr;
};
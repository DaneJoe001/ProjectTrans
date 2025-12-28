#pragma once

#include <QDialog>
#include <QPointer>
#include <QPushButton>

#include "danejoe/network/url/url_resolver.hpp"

#include "view/event/view_event_hub.hpp"

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
    ConnectionTestDialog(
        QPointer<ViewEventHub> view_event_hub,
        QWidget* parent = nullptr);
    ~ConnectionTestDialog();
    /**
     * @brief 初始化
     */
    void init();
    QPointer<QPushButton> get_send_push_button();
    QPointer<QLineEdit> get_url_line_edit();
    void set_recevied_text(const std::string& text);
public slots:
    /**
     * @brief 发送按钮点击
     */
    void on_send_push_button_clicked();
    void on_test_reponse(
        EventEnvelope event_envelope,
        TransContext trans_context,
        TestResponseTransfer response);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    QPointer<ViewEventHub> m_view_event_hub;
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
    DaneJoe::UrlResolver m_url_resolver;
};
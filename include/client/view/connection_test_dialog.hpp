#pragma once

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

class ConnectionTestDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectionTestDialog(QWidget* parent = nullptr);
    void init();
    void timerEvent(QTimerEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
signals:
    void send_data_signal(const std::vector<uint8_t>& data);
public slots:
    void on_send_push_button_clicked();
    void on_connect_push_button_clicked();
    void on_message_received(const std::vector<uint8_t>& data);
private:
    QLabel* m_url_label = nullptr;
    QLabel* m_send_label = nullptr;
    QLabel* m_recv_label = nullptr;
    QLineEdit* m_url_line_edit = nullptr;
    QPushButton* m_send_push_button = nullptr;
    QPushButton* m_connect_push_button = nullptr;
    QTextEdit* m_send_text_edit = nullptr;
    QTextBrowser* m_recv_text_browser = nullptr;
    QHBoxLayout* m_url_layout = nullptr;
    QWidget* m_url_widget = nullptr;
    QVBoxLayout* m_main_layout = nullptr;
    // std::unique_ptr<ClientConnection> m_connection;
    ConnectionThread* m_connection_thread = nullptr;
};
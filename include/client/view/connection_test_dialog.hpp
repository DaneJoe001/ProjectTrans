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

class ConnectionTestDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectionTestDialog(QWidget* parent = nullptr);
    void init();
    void timerEvent(QTimerEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
signals:
    void message_received(const std::vector<uint8_t>& data);
public slots:
    void on_send_push_button_clicked();
    void on_message_received(const std::vector<uint8_t>& data);
private:
    QLabel* m_url_label;
    QLabel* m_send_label;
    QLabel* m_recv_label;
    QLineEdit* m_url_line_edit;
    QPushButton* m_send_push_button;
    QTextEdit* m_send_text_edit;
    QTextBrowser* m_recv_text_browser;
    QHBoxLayout* m_url_layout;
    QWidget* m_url_widget;
    QVBoxLayout* m_main_layout;
    std::unique_ptr<ClientConnection> m_connection;
};
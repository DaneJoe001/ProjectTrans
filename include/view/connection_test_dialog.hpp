#pragma once

#include <QDialog>

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
signals:
    void send_data(const std::vector<uint8_t>& data);
public slots:
    void on_send_push_button_clicked();
    void on_message_received(const std::vector<uint8_t>& data);
private:
    QLabel* m_ip_label;
    QLabel* m_port_label;
    QLabel* m_send_label;
    QLabel* m_recv_label;
    QLineEdit* m_ip_line_edit;
    QLineEdit* m_port_line_edit;
    QPushButton* m_send_push_button;
    QTextEdit* m_send_text_edit;
    QTextBrowser* m_recv_text_browser;
    QHBoxLayout* m_ip_port_layout;
    QWidget* m_ip_port_widget;
    QVBoxLayout* m_main_layout;
};
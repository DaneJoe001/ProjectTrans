#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
#include<QTextBrowser>
#include<QHBoxLayout>
#include<QWidget>
#include<QVBoxLayout>
#include<QTextEdit>

#include "view/connection_test_dialog.hpp"
#include "log/manage_logger.hpp"

ConnectionTestDialog::ConnectionTestDialog(QWidget* parent)
    : QDialog(parent)
{
    m_main_layout = new QVBoxLayout(this);

    m_recv_label = new QLabel("Received TextBrowser", this);
    m_recv_text_browser = new QTextBrowser(this);

    m_recv_text_browser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    m_send_label = new QLabel("Send TextBrowser", this);
    m_send_text_edit = new QTextEdit(this);

    m_ip_label = new QLabel("IP:", this);
    m_ip_line_edit = new QLineEdit(this);
    m_port_label = new QLabel("Port:", this);
    m_port_line_edit = new QLineEdit(this);
    m_send_push_button = new QPushButton("Send", this);

    m_ip_port_widget = new QWidget(this);
    m_ip_port_layout = new QHBoxLayout(m_ip_port_widget);
    m_ip_port_layout->addWidget(m_ip_label);
    m_ip_port_layout->addWidget(m_ip_line_edit);
    m_ip_port_layout->addWidget(m_port_label);
    m_ip_port_layout->addWidget(m_port_line_edit);
    m_ip_port_layout->addWidget(m_send_push_button);
    m_ip_port_layout->setStretch(0, 1);
    m_ip_port_layout->setStretch(1, 4);
    m_ip_port_layout->setStretch(2, 2);
    m_ip_port_layout->setStretch(3, 2);
    m_ip_port_layout->setStretch(4, 2);

    m_main_layout->addWidget(m_recv_label);
    m_main_layout->addWidget(m_recv_text_browser);
    m_main_layout->addWidget(m_ip_port_widget);
    m_main_layout->addWidget(m_send_label);
    m_main_layout->addWidget(m_send_text_edit);

    connect(m_send_push_button, &QPushButton::clicked, this, &ConnectionTestDialog::on_send_push_button_clicked);
}

void ConnectionTestDialog::on_send_push_button_clicked()
{
    QString text = m_send_text_edit->toPlainText();
    m_send_text_edit->clear();
    auto data = text.toUtf8();
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "on_send_push_button_clicked():{}", text.toStdString());
    emit(send_data(std::vector<uint8_t>(data.begin(), data.end())));
}

void ConnectionTestDialog::on_message_received(const std::vector<uint8_t>& data)
{
    QString text = QString::fromUtf8(QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    m_recv_text_browser->append(text);
}
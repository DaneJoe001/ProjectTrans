#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
#include<QTextBrowser>
#include<QHBoxLayout>
#include<QWidget>
#include<QVBoxLayout>
#include<QTextEdit>

#include "common/network/url_resolver.hpp"
#include "client/view/connection_test_dialog.hpp"
#include "log/manage_logger.hpp"
#include "client/connect/connection_manager.hpp"

ConnectionTestDialog::ConnectionTestDialog(QWidget* parent)
    : QDialog(parent) {
}

void ConnectionTestDialog::init()
{
    m_main_layout = new QVBoxLayout(this);

    m_recv_label = new QLabel("Received TextBrowser", this);
    m_recv_text_browser = new QTextBrowser(this);

    m_recv_text_browser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    m_send_label = new QLabel("Send TextBrowser", this);
    m_send_text_edit = new QTextEdit(this);

    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
    m_send_push_button = new QPushButton("Send", this);

    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);
    m_url_layout->addWidget(m_url_label);
    m_url_layout->addWidget(m_url_line_edit);
    m_url_layout->addWidget(m_send_push_button);
    m_url_layout->setStretch(0, 1);
    m_url_layout->setStretch(1, 8);
    m_url_layout->setStretch(4, 1);

    m_main_layout->addWidget(m_recv_label);
    m_main_layout->addWidget(m_recv_text_browser);
    m_main_layout->addWidget(m_url_widget);
    m_main_layout->addWidget(m_send_label);
    m_main_layout->addWidget(m_send_text_edit);

    this->setGeometry(450, 250, 400, 500);

    m_url_line_edit->setText(QString::fromStdString("danejoe://127.0.0.1:8080"));

    connect(m_send_push_button, &QPushButton::clicked, this, &ConnectionTestDialog::on_send_push_button_clicked);
    connect(this, &ConnectionTestDialog::message_received, this, &ConnectionTestDialog::on_message_received);

    startTimer(1000);
}

void ConnectionTestDialog::on_send_push_button_clicked()
{
    QString text = m_send_text_edit->toPlainText();
    m_send_text_edit->clear();
    auto data = text.toUtf8();
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "on_send_push_button_clicked():{}", text.toStdString());
    QString url = m_url_line_edit->text();
    UrlResolver::UrlInfo info = UrlResolver::parse(url.toStdString());

    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "URL Parsed:{}:{}:{}", UrlResolver::to_string(info.protocol), info.ip, info.port);

    m_connection = std::move(ConnectionManager::get_instance().get_connection(info.ip, info.port));
    if (!m_connection)
    {
        ConnectionManager::get_instance().add_connection(info.ip, info.port);
        m_connection = std::move(ConnectionManager::get_instance().get_connection(info.ip, info.port));
    }
    if (m_connection)
    {
        m_connection->send(std::vector<uint8_t>(data.begin(), data.end()));
    }
}

void ConnectionTestDialog::on_message_received(const std::vector<uint8_t>& data)
{
    QString text = QString::fromUtf8(QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    m_recv_text_browser->append(text);
}

void ConnectionTestDialog::closeEvent(QCloseEvent* event)
{
    if (m_connection)
    {
        ConnectionManager::get_instance().recycle_connection(std::move(m_connection));
    }
}

void ConnectionTestDialog::timerEvent(QTimerEvent* event)
{
    if (m_connection)
    {
        std::vector<uint8_t> data = m_connection->recieve();
        if (!data.empty())
        {
            emit message_received(data);
        }
    }
}
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
#include<QTextBrowser>
#include<QHBoxLayout>
#include<QWidget>
#include<QVBoxLayout>
#include<QTextEdit>
#include<QMessageBox>

#include "common/network/url_resolver.hpp"
#include "client/view/connection_test_dialog.hpp"
#include "log/manage_logger.hpp"
#include "client/connect/connection_manager.hpp"
#include "client/connect/connection_thread.hpp"
#include "common/util/screen_util.hpp"

ConnectionTestDialog::ConnectionTestDialog(QWidget* parent)
    : QDialog(parent) {
}

void ConnectionTestDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ConnectionTestDialog", "ConnectionTestDialog has been initialized");
        return;
    }
    m_main_layout = new QVBoxLayout(this);

    m_recv_label = new QLabel("Received TextBrowser", this);
    m_recv_text_browser = new QTextBrowser(this);

    m_recv_text_browser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    m_send_label = new QLabel("Send TextBrowser", this);
    m_send_text_edit = new QTextEdit(this);

    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
    m_send_push_button = new QPushButton("Send", this);
    m_connect_push_button = new QPushButton("Connect", this);

    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);
    m_url_layout->addWidget(m_url_label);
    m_url_layout->addWidget(m_url_line_edit);
    m_url_layout->addWidget(m_connect_push_button);
    m_url_layout->addWidget(m_send_push_button);
    m_url_layout->setStretch(0, 1);
    m_url_layout->setStretch(1, 8);
    m_url_layout->setStretch(2, 1);
    m_url_layout->setStretch(3, 1);

    m_main_layout->addWidget(m_recv_label);
    m_main_layout->addWidget(m_recv_text_browser);
    m_main_layout->addWidget(m_url_widget);
    m_main_layout->addWidget(m_send_label);
    m_main_layout->addWidget(m_send_text_edit);

    ScreenUtil::RectInfo screen_rect = { 450, 250, 500, 400 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);
        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "Window rect: {}", screen_rect.to_string());
    m_connection_thread = new ConnectionThread(this);

    /// @note 自定义服务端测试链接
    m_url_line_edit->setText(QString::fromStdString("danejoe://127.0.0.1:8080"));

    connect(m_connection_thread, &ConnectionThread::data_received_signal, this, &ConnectionTestDialog::on_message_received);
    connect(this, &ConnectionTestDialog::send_data_signal, m_connection_thread, &ConnectionThread::data_send_slot);
    connect(m_send_push_button, &QPushButton::clicked, this, &ConnectionTestDialog::on_send_push_button_clicked);
    connect(m_connect_push_button, &QPushButton::clicked, this, &ConnectionTestDialog::on_connect_push_button_clicked);

    startTimer(1000);
    m_is_init = true;
}

void ConnectionTestDialog::on_send_push_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ConnectionTestDialog", "ConnectionTestDialog has not been initialized");
        return;
    }
    QString text = m_send_text_edit->toPlainText();
    m_send_text_edit->clear();
    auto data = text.toUtf8();
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "on_send_push_button_clicked():{}", text.toStdString());
    if (!m_connection_thread)
    {
        QMessageBox::warning(this, "Error", "Not connected");
        return;
    }

    emit(send_data_signal(std::vector<uint8_t>(data.begin(), data.end())));

}

void ConnectionTestDialog::on_connect_push_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ConnectionTestDialog", "ConnectionTestDialog has not been initialized");
        return;
    }
    QString url = m_url_line_edit->text();
    UrlResolver::UrlInfo info = UrlResolver::parse(url.toStdString());

    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "URL Parsed:{}:{}:{}", UrlResolver::to_string(info.protocol), info.ip, info.port);

    bool is_connect = m_connection_thread->init(info.ip, info.port);
    if (is_connect)
    {
        m_connection_thread->start();
        QMessageBox::information(this, "Success", "Connected");
    }
    else
    {
        QMessageBox::warning(this, "Error", "Failed to connect");
    }
}

void ConnectionTestDialog::on_message_received(const std::vector<uint8_t>& data)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ConnectionTestDialog", "ConnectionTestDialog has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "on_message_received()");
    QString text = QString::fromUtf8(QByteArray(reinterpret_cast<const char*>(data.data()), data.size()));
    m_recv_text_browser->append(text);
}

void ConnectionTestDialog::closeEvent(QCloseEvent* event)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ConnectionTestDialog", "ConnectionTestDialog has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "closeEvent()");
    if (m_connection_thread)
    {
        m_connection_thread->deinit();
    }
}

void ConnectionTestDialog::timerEvent(QTimerEvent* event)
{

}
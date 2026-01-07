#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
#include<QTextBrowser>
#include<QHBoxLayout>
#include<QWidget>
#include<QVBoxLayout>
#include<QTextEdit>
#include <QMessageBox>

#include "danejoe/logger/logger_manager.hpp"	
#include "danejoe/common/core/variable_util.hpp"

#include "common/protocol/network_endpoint.hpp"
#include "common/util/screen_util.hpp"
#include "model/transfer/test_transfer.hpp"
#include "view/dialog/connection_test_dialog.hpp"

ConnectionTestDialog::ConnectionTestDialog(QPointer<ViewEventHub> view_event_hub, QWidget* parent)
    : QDialog(parent), m_view_event_hub(view_event_hub)
{}

ConnectionTestDialog::~ConnectionTestDialog() {}

void ConnectionTestDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ConnectionTestDialog", "ConnectionTestDialog has been initialized");
        return;
    }
    this->setObjectName("ConnectionTestDialog");
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
    m_url_layout->setStretch(2, 1);

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

    /// @note 自定义服务端测试链接
    m_url_line_edit->setText(QString::fromStdString("danejoe://127.0.0.1:8080"));

    connect(m_view_event_hub, &ViewEventHub::test_response, this, &ConnectionTestDialog::on_test_reponse);
    m_is_init = true;
    connect(m_send_push_button, &QPushButton::clicked, this, &ConnectionTestDialog::on_send_push_button_clicked);
}

void ConnectionTestDialog::on_send_push_button_clicked()
{
    DANEJOE_LOG_TRACE("default", "ConnectionTestDialog", "Send push button clicked!");
    auto url = m_url_line_edit->text().toStdString();
    auto url_info = m_url_resolver.parse(url);
    NetworkEndpoint endpoint = { url_info.host, url_info.port };
    auto message = m_send_text_edit->toPlainText().toStdString();
    TestRequestTransfer request = { message };
    EventContext event_context = { this->objectName().toStdString() };
    if (!m_view_event_hub)
    {
        DANEJOE_LOG_WARN("default", "ConnectionTestDialog", "View event hub is null!");
        return;
    }
    m_view_event_hub->publish_test_request(event_context, endpoint, request);
    m_send_text_edit->clear();
}

void ConnectionTestDialog::on_test_reponse(
    EventEnvelope event_envelope,
    TransContext trans_context,
    TestResponseTransfer response)
{
    DANEJOE_UNUSED(event_envelope)
    DANEJOE_UNUSED(trans_context)

    set_recevied_text(response.message);
}

void ConnectionTestDialog::set_recevied_text(const std::string& text)
{
    if (m_recv_text_browser)
    {
        m_recv_text_browser->append(QString::fromStdString(text));
    }
}
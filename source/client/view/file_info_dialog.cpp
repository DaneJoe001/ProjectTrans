#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "client/view/file_info_dialog.hpp"
#include "log/manage_logger.hpp"
#include "common/util/screen_util.hpp"

FileInfoDialog::FileInfoDialog(QWidget* parent) :QDialog(parent)
{

}
void FileInfoDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "FileInfoDialog", "FileInfoDialog has already been initialized");
        return;
    }
    ScreenUtil::RectInfo screen_rect = { 450, 250, 400, 300 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);
        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    DANEJOE_LOG_TRACE("default", "FileInfoDialog", "Window rect: {}", screen_rect.to_string());

    m_main_layout = new QVBoxLayout(this);
    m_stack_widget = new QStackedWidget(this);
    m_main_layout->addWidget(m_stack_widget);

    m_waiting_widget = new QWidget(this);
    m_stack_widget->addWidget(m_waiting_widget);
    m_waiting_layout = new QVBoxLayout(m_waiting_widget);

    m_waiting_text_label = new QLabel(m_waiting_widget);
    m_waiting_icon_label = new QLabel(m_waiting_widget);
    m_waiting_cancel_button = new QPushButton("Cancel", m_waiting_widget);
    m_waiting_layout->addWidget(m_waiting_icon_label);
    m_waiting_layout->addWidget(m_waiting_text_label);
    m_waiting_layout->addWidget(m_waiting_cancel_button);
    m_waiting_layout->setStretch(0, 8);
    m_waiting_layout->setStretch(1, 1);
    m_waiting_layout->setStretch(2, 1);

    m_info_widget = new QWidget(this);
    m_stack_widget->addWidget(m_info_widget);
    m_info_layout = new QVBoxLayout(m_info_widget);

    m_info_text_browser = new QTextBrowser(m_info_widget);

    m_info_button_widget = new QWidget(m_info_widget);
    m_info_button_layout = new QHBoxLayout(m_info_button_widget);
    m_info_ok_button = new QPushButton("OK", m_info_button_widget);
    m_info_cancel_button = new QPushButton("Cancel", m_info_button_widget);
    m_info_button_layout->addWidget(m_info_cancel_button);
    m_info_button_layout->addWidget(m_info_ok_button);
    m_info_button_layout->setStretch(0, 1);
    m_info_button_layout->setStretch(1, 1);

    m_info_layout->addWidget(m_info_text_browser);
    m_info_layout->addWidget(m_info_button_widget);
    m_info_layout->setStretch(0, 8);
    m_info_layout->setStretch(1, 2);

    m_error_widget = new QWidget(this);
    m_stack_widget->addWidget(m_error_widget);
    m_error_layout = new QVBoxLayout(m_error_widget);

    m_stack_widget->setCurrentIndex(0);
    connect(m_info_ok_button, &QPushButton::clicked, this, &FileInfoDialog::on_info_ok_button_clicked);
    connect(m_waiting_cancel_button, &QPushButton::clicked, this, &FileInfoDialog::on_waiting_cancel_button_clicked);
    m_is_init = true;
    connect(m_info_cancel_button, &QPushButton::clicked, this, &FileInfoDialog::on_info_cancel_button_clicked);
}

void FileInfoDialog::on_received_raw_file_info(std::vector<uint8_t> info)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    m_raw_file_info = std::string(info.begin(), info.end());
    /// @todo 使用富文本展示文件信息
    m_info_text_browser->setText(QString::fromStdString(m_raw_file_info));
    // 切换到文件信息界面
    m_stack_widget->setCurrentIndex(1);
}

void FileInfoDialog::on_waiting_cancel_button_clicked()
{
    /// @todo 取消传输
    this->hide();
}

void FileInfoDialog::on_info_ok_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    emit info_ok_button_clicked(m_raw_file_info);
    m_raw_file_info.clear();
    this->hide();
}

void FileInfoDialog::on_info_cancel_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    m_raw_file_info.clear();
    this->hide();
}
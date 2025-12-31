#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

#include "danejoe/logger/logger_manager.hpp"

#include "common/util/screen_util.hpp"
#include "view/dialog/task_info_dialog.hpp"

TaskInfoDialog::TaskInfoDialog(QPointer<ViewEventHub> view_event_hub, QWidget* parent) :QDialog(parent), m_view_event_hub(view_event_hub) {}

void TaskInfoDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "TaskInfoDialog", "TaskInfoDialog has already been initialized");
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
    DANEJOE_LOG_TRACE("default", "TaskInfoDialog", "Window rect: {}", screen_rect.to_string());

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

    m_info_saved_path_widget = new QWidget(m_info_widget);
    m_info_saved_path_layout = new QHBoxLayout(m_info_saved_path_widget);
    m_info_saved_path_label = new QLabel("Saved Path:", m_info_saved_path_widget);
    m_info_saved_path_line_edit = new QLineEdit(m_info_saved_path_widget);
    m_info_saved_path_line_edit->setText("/home/danejoe001/personal_code/code_cpp_project/cpp_project_trans/resource/client/download/default.test");
    m_info_saved_path_button = new QPushButton("...", m_info_saved_path_widget);
    m_info_saved_path_layout->addWidget(m_info_saved_path_label);
    m_info_saved_path_layout->addWidget(m_info_saved_path_line_edit);
    m_info_saved_path_layout->addWidget(m_info_saved_path_button);
    m_info_saved_path_layout->setStretch(0, 1);
    m_info_saved_path_layout->setStretch(1, 6);
    m_info_saved_path_layout->setStretch(2, 1);

    m_info_layout->addWidget(m_info_text_browser);
    m_info_layout->addWidget(m_info_saved_path_widget);
    m_info_layout->addWidget(m_info_button_widget);
    m_info_layout->setStretch(0, 8);
    m_info_layout->setStretch(1, 2);
    m_info_layout->setStretch(2, 2);

    m_error_widget = new QWidget(this);
    m_stack_widget->addWidget(m_error_widget);
    m_error_layout = new QVBoxLayout(m_error_widget);

    m_stack_widget->setCurrentIndex(0);
    m_is_init = true;
}

void TaskInfoDialog::switch_to_waiting_panel()
{
    m_stack_widget->setCurrentIndex(0);
}
void TaskInfoDialog::switch_to_info_panel()
{
    m_stack_widget->setCurrentIndex(1);
}

void TaskInfoDialog::on_download_response(EventEnvelope event_envelope,
    TransContext trans_context,
    DownloadResponseTransfer response)
{

}
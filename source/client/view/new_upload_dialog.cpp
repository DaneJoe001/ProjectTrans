#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QProgressBar>
#include <QFileDialog>

#include "client/view/new_upload_dialog.hpp"
#include "common/log/manage_logger.hpp"
#include "common/util/screen_util.hpp"

NewUploadDialog::NewUploadDialog(QWidget* parent) :QDialog(parent)
{

}

void NewUploadDialog::init()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_WARN("default", "NewUploadDialog", "NewUploadDialog is already initialized");
        return;
    }
    this->setWindowTitle("New Upload");
    ScreenUtil::RectInfo screen_rect = { 450,250,400,400 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);
        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    DANEJOE_LOG_TRACE("default", "NewUploadDialog", "Window rect: {}", screen_rect.to_string());
    m_main_layout = new QVBoxLayout(this);

    m_file_dialog = new QFileDialog(this);
    m_file_path_widget = new QWidget(this);
    m_file_path_layout = new QHBoxLayout(m_file_path_widget);
    m_file_path_label = new QLabel("File Path:", this);
    m_file_path_line_edit = new QLineEdit(this);
    m_path_select_push_button = new QPushButton("...", this);
    m_add_file_push_button = new QPushButton("Add File", this);
    m_file_path_layout->addWidget(m_file_path_label);
    m_file_path_layout->addWidget(m_file_path_line_edit);
    m_file_path_layout->addWidget(m_path_select_push_button);
    m_file_path_layout->addWidget(m_add_file_push_button);
    m_file_path_layout->setStretch(0, 1);
    m_file_path_layout->setStretch(1, 7);
    m_file_path_layout->setStretch(2, 1);
    m_file_path_layout->setStretch(3, 1);


    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);
    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
    m_upload_push_button = new QPushButton("Upload", this);

    m_url_layout->addWidget(m_url_label);
    m_url_layout->addWidget(m_url_line_edit);
    m_url_layout->addWidget(m_upload_push_button);
    m_url_layout->setStretch(0, 1);
    m_url_layout->setStretch(1, 8);
    m_url_layout->setStretch(2, 1);

    m_user_info_widget = new QWidget(this);
    m_user_info_layout = new QHBoxLayout(m_user_info_widget);
    m_username_label = new QLabel("Username:", this);
    m_username_line_edit = new QLineEdit(this);
    m_password_label = new QLabel("Password:", this);
    m_password_line_edit = new QLineEdit(this);
    m_password_line_edit->setEchoMode(QLineEdit::Password);
    m_user_info_layout->addWidget(m_username_label);
    m_user_info_layout->addWidget(m_username_line_edit);
    m_user_info_layout->addWidget(m_password_label);
    m_user_info_layout->addWidget(m_password_line_edit);
    m_user_info_layout->setStretch(0, 1);
    m_user_info_layout->setStretch(1, 2);
    m_user_info_layout->setStretch(2, 1);
    m_user_info_layout->setStretch(3, 2);

    m_upload_progress_bar = new QProgressBar(this);
    m_upload_progress_bar->setRange(0, 100);

    m_upload_info_browser = new QTextBrowser(this);

    m_main_layout->addWidget(m_file_path_widget);
    m_main_layout->addWidget(m_url_widget);
    m_main_layout->addWidget(m_user_info_widget);
    m_main_layout->addWidget(m_upload_progress_bar);
    m_main_layout->addWidget(m_upload_info_browser);
    m_main_layout->setStretch(0, 1);
    m_main_layout->setStretch(1, 1);
    m_main_layout->setStretch(2, 1);
    m_main_layout->setStretch(3, 1);
    m_main_layout->setStretch(4, 7);
    m_is_init = true;
}
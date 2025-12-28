#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QProgressBar>
#include <QMessageBox>
#include <QDateTime>

#include <danejoe/logger/logger_manager.hpp>

#include "common/util/screen_util.hpp"
#include "view/dialog/new_download_dialog.hpp"
#include "view/dialog/task_info_dialog.hpp"

NewDownloadDialog::NewDownloadDialog(QWidget* parent) :QDialog(parent) {}

void NewDownloadDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog", "NewDownloadDialog has been initialized");
        return;
    }
    this->setWindowTitle("New Download");
    ScreenUtil::RectInfo screen_rect = { 450, 250, 600, 400 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);

        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    DANEJOE_LOG_TRACE("default", "NewDownloadDialog", "Window rect: {}", screen_rect.to_string());
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    m_main_layout = new QVBoxLayout(this);

    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);
    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
    /// @note 自定义服务端测试链接
    m_url_line_edit->setText("danejoe://127.0.0.1:8080/download?file_id=1");
    m_add_download_push_button = new QPushButton("Add download", this);

    m_url_layout->addWidget(m_url_label);
    m_url_layout->addWidget(m_url_line_edit);
    m_url_layout->addWidget(m_add_download_push_button);
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

    m_download_progress_bar = new QProgressBar(this);
    m_download_progress_bar->setRange(0, 100);

    m_download_info_browser = new QTextBrowser(this);

    m_main_layout->addWidget(m_url_widget);
    m_main_layout->addWidget(m_user_info_widget);
    m_main_layout->addWidget(m_download_progress_bar);
    m_main_layout->addWidget(m_download_info_browser);
    m_main_layout->setStretch(0, 1);
    m_main_layout->setStretch(1, 1);
    m_main_layout->setStretch(2, 1);
    m_main_layout->setStretch(3, 7);

    m_file_info_dialog = new TaskInfoDialog(this);
    m_file_info_dialog->init();

    m_is_init = true;
}
NewDownloadDialog::~NewDownloadDialog() {}
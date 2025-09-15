#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QProgressBar>

#include "client/view/new_download_dialog.hpp"
#include "client/model/trans_manager.hpp"
#include "log/manage_logger.hpp"

NewDownloadDialog::NewDownloadDialog(QWidget* parent) :QDialog(parent)
{

}

void NewDownloadDialog::init()
{
    this->setWindowTitle("New Download");
    this->setGeometry(450, 250, 400, 400);
    m_main_layout = new QVBoxLayout(this);

    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);
    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
    /// @note 自定义服务端测试链接
    m_url_line_edit->setText("danejoe://127.0.0.1:8080/download?file_id=1");
    m_download_push_button = new QPushButton("Download", this);

    m_url_layout->addWidget(m_url_label);
    m_url_layout->addWidget(m_url_line_edit);
    m_url_layout->addWidget(m_download_push_button);
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

    m_trans_manager = TransManager::get_instance();

    connect(m_download_push_button, &QPushButton::clicked, this, &NewDownloadDialog::on_download_push_button_clicked);
}

void NewDownloadDialog::on_download_push_button_clicked()
{
    DANEJOE_LOG_TRACE("default", "NewDownloadDialog", "on_download_push_button_clicked");
    QString url = m_url_line_edit->text();
    QString username = m_username_line_edit->text();
    QString password = m_password_line_edit->text();
    DownloadRequest request;
    request.file_url = url.toStdString();
    request.username = username.toStdString();
    request.password = password.toStdString();
    m_trans_manager->add_download(request);
}
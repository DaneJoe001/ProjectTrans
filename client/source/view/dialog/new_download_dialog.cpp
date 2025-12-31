#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QFileDialog>

#include <danejoe/logger/logger_manager.hpp>

#include "common/util/screen_util.hpp"
#include "view/dialog/new_download_dialog.hpp"
#include "view/dialog/task_info_dialog.hpp"

NewDownloadDialog::NewDownloadDialog(
    TaskService& task_service,
    ClientFileService& client_file_service,
    BlockService& block_service,
    QPointer<ViewEventHub> view_event_hub,
    QWidget* parent)
    : QDialog(parent),
    m_view_event_hub(view_event_hub),
    m_task_service(task_service),
    m_client_file_service(client_file_service),
    m_block_service(block_service)
{}
NewDownloadDialog::~NewDownloadDialog() {}

void NewDownloadDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog",
            "NewDownloadDialog has been initialized");
        return;
    }
    this->setWindowTitle("New Download");
    ScreenUtil::RectInfo screen_rect = { 450, 250, 600, 400 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(
            parent_window->geometry(), screen_rect,
            ScreenUtil::RealativePosition::Center);

        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    DANEJOE_LOG_TRACE("default", "NewDownloadDialog", "Window rect: {}",
        screen_rect.to_string());
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x,
        screen_rect.size.y);

    m_message_box = new QMessageBox(this);
    m_path_check_message_box = new QMessageBox(this);

    m_main_layout = new QVBoxLayout(this);

    m_url_widget = new QWidget(this);
    m_url_layout = new QHBoxLayout(m_url_widget);

    m_url_label = new QLabel("URL:", this);
    m_url_line_edit = new QLineEdit(this);
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

    m_saved_path_widget = new QWidget(this);
    m_saved_path_layout = new QHBoxLayout(m_saved_path_widget);
    m_saved_path_label = new QLabel("Saved Path:", m_saved_path_widget);
    m_saved_path_line_edit = new QLineEdit(m_saved_path_widget);
    m_saved_path_line_edit->setText(
        "/home/danejoe001/personal_code/code_cpp_project/cpp_project_trans/client/resource/download/default.test");
    m_saved_path_button = new QPushButton("...", m_saved_path_widget);
    m_saved_path_layout->addWidget(m_saved_path_label);
    m_saved_path_layout->addWidget(m_saved_path_line_edit);
    m_saved_path_layout->addWidget(m_saved_path_button);
    m_saved_path_layout->setStretch(0, 1);
    m_saved_path_layout->setStretch(1, 6);
    m_saved_path_layout->setStretch(2, 1);


    m_download_info_browser = new QTextBrowser(this);

    m_main_layout->addWidget(m_url_widget);
    m_main_layout->addWidget(m_user_info_widget);
    m_main_layout->addWidget(m_saved_path_widget);
    m_main_layout->addWidget(m_download_info_browser);
    m_main_layout->setStretch(0, 1);
    m_main_layout->setStretch(1, 1);
    m_main_layout->setStretch(2, 1);
    m_main_layout->setStretch(4, 7);

    m_task_info_dialog = new TaskInfoDialog(m_view_event_hub, this);
    m_task_info_dialog->init();

    connect(m_add_download_push_button, &QPushButton::clicked, this,
        &NewDownloadDialog::on_add_download_push_button_clicked);
    connect(m_view_event_hub, &ViewEventHub::download_response, this,
        &NewDownloadDialog::on_download_response);
    connect(m_saved_path_button, &QPushButton::clicked, this, &NewDownloadDialog::on_path_button_clicked);

    m_is_init = true;
}

void NewDownloadDialog::on_add_download_push_button_clicked()
{
    /// @todo 创建任务，发送请求--》等待面板-->>区分超时和正常
    auto url_text = m_url_line_edit->text().toStdString();
    auto url_info = m_url_resolver.parse(url_text);
    EventContext event_context;
    event_context.m_object_name = "NewDownloadDialog";
    NetworkEndpoint endpoint;
    endpoint.ip = url_info.host;
    endpoint.port = url_info.port;
    DownloadRequestTransfer request;
    auto file_id_opt = url_info.get_param("file_id");
    if (!file_id_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog",
            "Failed to add download: No enough params");
        return;
    }
    auto saved_path_qstr = m_saved_path_line_edit->text();
    /// @todo 考虑重复路径校验
    QFileInfo file_info(saved_path_qstr);

    if (file_info.exists())
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog", "File already exists");
        auto reply = m_path_check_message_box->question(
            this, "File already exists",
            "File already exists, do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);
        if (reply != QMessageBox::Yes)
        {
            return;
        }
    }

    if (!file_info.dir().exists())
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog",
            "Failed to add download: Directory does not exist");
        auto reply = m_path_check_message_box->question(
            this, "Directory does not exist",
            "Directory does not exist, do you want to create it?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);
        if (reply != QMessageBox::Yes)
        {
            return;
        }
        file_info.dir().mkpath(file_info.dir().path());
    }
    TaskEntity task_entity;
    task_entity.file_id = std::stoll(file_id_opt.value());

    task_entity.saved_path = file_info.filePath().toStdString();
    task_entity.source_url = url_text;
    task_entity.operation = Operation::Download;
    task_entity.state = TaskState::Waiting;
    task_entity.start_time = std::chrono::system_clock::now();

    request.file_id = task_entity.file_id;

    if (!m_task_service.add(task_entity))
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog", "Failed to add task into database");
        return;
    }
    m_view_event_hub->publish_task_entity_add(task_entity);
    request.task_id = task_entity.task_id;
    DANEJOE_LOG_INFO("default", "NewDownloadDialog", "Request task_id: {}", request.task_id);
    m_view_event_hub->publish_download_request(event_context, endpoint, request);
}
void NewDownloadDialog::on_ok_to_add_file_button_clicked() {}

void NewDownloadDialog::on_download_response(
    EventEnvelope event_envelope, TransContext trans_context,
    DownloadResponseTransfer response)
{
    DANEJOE_LOG_INFO("default", "NewDownloadDialog",
        "Source from: {}",
        event_envelope.m_event_context.m_object_name);
    DANEJOE_LOG_INFO("default", "NewDownloadDialog",
        "Recieved response: {}", response.to_string());
    DANEJOE_LOG_INFO("default", "NewDownloadDialog",
        "Trans context: {}", trans_context.request_id);
    /// @todo 显示请求的文件信息
    ClientFileEntity file_entity;
    file_entity.file_id = response.file_id;
    file_entity.file_name = response.file_name;
    file_entity.file_size = response.file_size;
    file_entity.md5_code = response.md5_code;

    if (file_entity.file_id < 0)
    {
        DANEJOE_LOG_WARN("default", "NewDownloadDialog", "Failed to get file info");
        return;
    }

    auto bytes_to_human = [](int64_t bytes) -> QString
        {
            const double b = static_cast<double>(bytes);
            if (b < 1024.0)
            {
                return QString::number(bytes) + " B";
            }
            if (b < 1024.0 * 1024.0)
            {
                return QString("%1 KB").arg(b / 1024.0, 0, 'f', 2);
            }
            if (b < 1024.0 * 1024.0 * 1024.0)
            {
                return QString("%1 MB").arg(b / (1024.0 * 1024.0), 0, 'f', 2);
            }
            return QString("%1 GB").arg(b / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
        };

    const QString file_name = QString::fromStdString(response.file_name).toHtmlEscaped();
    const QString md5_code = QString::fromStdString(response.md5_code).toHtmlEscaped();
    const QString file_size_human = bytes_to_human(response.file_size);
    const QString file_size_raw = QString::number(response.file_size);

    QString file_info_text =
        QString(
        "<div style='font-family:Segoe UI, Arial; line-height:1.45;'>"
        "  <div style='display:flex; align-items:center; gap:10px;'>"
        "    <span style='font-size:18px; font-weight:700; color:#1f2937;'>Download File Info</span>"
        "    <span style='font-size:12px; color:#6b7280;'>response received</span>"
        "  </div>"
        "  <div style='height:8px;'></div>"
        "  <table style='border-collapse:collapse; width:100%;'>"
        "    <tr><td style='padding:6px 10px; color:#6b7280; width:120px;'>Task ID</td><td style='padding:6px 10px; font-weight:600; color:#111827;'>%1</td></tr>"
        "    <tr><td style='padding:6px 10px; color:#6b7280;'>File ID</td><td style='padding:6px 10px; font-weight:600; color:#111827;'>%2</td></tr>"
        "    <tr><td style='padding:6px 10px; color:#6b7280;'>File Name</td><td style='padding:6px 10px; color:#111827;'>%3</td></tr>"
        "    <tr><td style='padding:6px 10px; color:#6b7280;'>File Size</td><td style='padding:6px 10px; color:#111827;'>%4 <span style='color:#9ca3af;'>(%5 bytes)</span></td></tr>"
        "    <tr><td style='padding:6px 10px; color:#6b7280;'>MD5</td><td style='padding:6px 10px; font-family:monospace; color:#111827;'>%6</td></tr>"
        "  </table>"
        "</div>")
        .arg(QString::number(response.task_id))
        .arg(QString::number(response.file_id))
        .arg(file_name)
        .arg(file_size_human)
        .arg(file_size_raw)
        .arg(md5_code);

    if (m_download_info_browser)
    {
        m_download_info_browser->setHtml(file_info_text);
    }
    DANEJOE_LOG_INFO("default", "NewDownloadDialog", "Recieved file info: {}", file_entity.to_string());
    m_client_file_service.add(file_entity);
    m_block_service.add(file_entity, response.task_id, BlockParam());
}

void NewDownloadDialog::on_path_button_clicked()
{
    QString saved_path = QFileDialog::getSaveFileName(this, "Save File", "/home/danejoe001/personal_code/code_cpp_project/cpp_project_trans/resource/client/download", "All Files (*)");
    m_saved_path_line_edit->setText(saved_path);
}
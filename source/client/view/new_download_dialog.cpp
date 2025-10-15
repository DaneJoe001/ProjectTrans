#include <chrono>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QProgressBar>
#include <QMessageBox>
#include <QDateTime>

#include "client/view/new_download_dialog.hpp"
#include "client/connect/trans_manager.hpp"
#include "log/manage_logger.hpp"
#include "client/connect/connection_thread.hpp"
#include "common/network/url_resolver.hpp"
#include "client/connect/message_handler.hpp"
#include "client/view/file_info_dialog.hpp"
#include "common/util/screen_util.hpp"
#include "client/model/file_trans_info_table_model.hpp"

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

    m_connection_thread = new ConnectionThread(this);
    m_connection_thread->start();

    m_file_info_dialog = new FileInfoDialog(this);
    m_file_info_dialog->init();
    m_file_info_dialog->set_operation(Operation::Download);

    m_file_info_service.init();
    m_block_request_info_service.init();

    m_is_handle_trans_file_info_thread_running = true;
    m_handle_trans_file_info_thread = std::thread([this]() {
        while (m_is_handle_trans_file_info_thread_running.load())
        {
            auto trans_info_optional = m_handle_trans_file_info_queue.pop();
            if (trans_info_optional.has_value())
            {
                auto trans_info = trans_info_optional.value();
                std::list<BlockRequestInfo> block_list = Client::MessageHandler::calculate_block_info(trans_info.file_info, trans_info.block_param_config);

                for (auto& block_info : block_list)
                {
                    m_block_request_info_service.add(block_info);
                }
                FileTransInfoTableModel::get_instance()->add(trans_info.file_info);
            }
        }
        });

    connect(m_add_download_push_button, &QPushButton::clicked, this, &NewDownloadDialog::on_download_push_button_clicked);
    // 连接文件信息对话框的文件信息设置和线程接收
    connect(m_connection_thread, &ConnectionThread::data_received_signal, m_file_info_dialog, &FileInfoDialog::on_received_raw_file_info);
    connect(m_file_info_dialog, &FileInfoDialog::info_ok_button_clicked, this, &NewDownloadDialog::ok_to_add_file_info);
    m_is_init = true;
}
NewDownloadDialog::~NewDownloadDialog()
{
    m_is_handle_trans_file_info_thread_running.store(false);
    m_handle_trans_file_info_queue.close();
    if (m_handle_trans_file_info_thread.joinable())
    {
        m_handle_trans_file_info_thread.join();
    }
}
void NewDownloadDialog::on_download_push_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "NewDownloadDialog", "NewDownloadDialog has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "NewDownloadDialog", "on_download_push_button_clicked");
    QString url = m_url_line_edit->text();
    QString username = m_username_line_edit->text();
    QString password = m_password_line_edit->text();
    std::string std_url = url.toStdString();
    std::string std_username = username.toStdString();
    std::string std_password = password.toStdString();
    // 展示正在获取文件信息等待对话框
    // 创建文件信息
    // connection_thread属于Qt对象类继承，需要放在Qt中进行处理

    // 需要一个请求信息类
    ClientFileInfo info;
    // 解析URL
    auto url_info = UrlResolver::parse(std_url);
    // 不需要根据不同的协议进行不同的处理，只需要向对应的IP和端口发送即可
    // 不过消息中可以追加信息在MessageHandler中构建
    // 设置源路径
    info.source_path = std_url;
    bool is_init = m_connection_thread->init(url_info.host, url_info.port);
    if (!is_init)
    {
        QMessageBox::warning(this, "Error", "Cannot connect to server.");
        return;
    }
    auto download_request = Client::MessageHandler::build_download_request(url_info, std_username, std_password);

    // 发送文件下载请求
    m_connection_thread->data_send_slot(download_request);

    // 显示文件信息对话框，进去默认为等待界面
    m_file_info_dialog->show();
}

void NewDownloadDialog::ok_to_add_file_info(ClientFileInfo file_info)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "NewDownloadDialog", "NewDownloadDialog has not been initialized");
        return;
    }
    file_info.operation = Operation::Download;
    file_info.source_path = m_url_line_edit->text().toStdString();

    /// @todo 使用其他方式持久化配置
    BlockParamConfig config;
    config.min_block_size = 1024 * 1024 * 30;

    // 当路径相同时且保存文件名相同时不执行添加
    auto data = m_file_info_service.get_by_saved_name_and_path(file_info.saved_name, file_info.saved_path);
    if (data.has_value())
    {
        QMessageBox::warning(this, "Error", "You can't add task with same saved name and path.");
        return;
    }
    bool is_added_info = m_file_info_service.add(file_info);
    if (!is_added_info)
    {
        QMessageBox::warning(this, "Error", "Cannot add file info.");
        return;
    }
    m_handle_trans_file_info_queue.push(TransFileInfo{ file_info,config });
    QString text = QString(R"(
        <!DOCTYPE html>
        <html lang="zh">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>文件信息展示</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    margin: 20px;
                }
                .file-info {
                    border: 1px solid #ccc;
                    padding: 15px;
                    border-radius: 5px;
                    background-color: #f9f9f9;
                }
                .file-info h2 {
                    margin-top: 0;
                }
                .file-info p {
                    margin: 5px 0;
                }
            </style>
        </head>
        <body>

            <div class="file-info">
                <h2>文件信息</h2>
                <p><strong>文件ID:</strong> <span id="file_id">%1</span></p>
                <p><strong>保存名称:</strong> <span id="saved_name">%2</span></p>
                <p><strong>源路径:</strong> <span id="source_path">%3</span></p>
                <p><strong>保存路径:</strong> <span id="saved_path">%4</span></p>
                <p><strong>文件大小:</strong> <span id="file_size">%5</span> 字节</p>
                <p><strong>操作:</strong> <span id="operation">%6</span></p>
                <p><strong>文件状态:</strong> <span id="state">%7</span></p>
                <p><strong>MD5码:</strong> <span id="md5_code">%8</span></p>
                <p><strong>创建时间:</strong> <span id="create_time">%9</span></p>
            </div>

        </body>
        </html>
        )").
        arg(file_info.file_id).
        arg(QString::fromStdString(file_info.saved_name)).
        arg(QString::fromStdString(file_info.source_path)).
        arg(QString::fromStdString(file_info.saved_path)).
        arg(file_info.file_size).
        arg(QString::fromStdString(to_string(file_info.operation))).
        arg(QString::fromStdString(to_string(file_info.state))).
        arg(QString::fromStdString(file_info.md5_code)).
        arg(QDateTime::fromSecsSinceEpoch(std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count()).toString("yyyy-MM-dd hh:mm:ss"));
    m_download_info_browser->setText(text);
    QMessageBox::information(this, "Success", "Add file info success. Please wait for moment.");
}
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

#include "client/view/file_info_dialog.hpp"
#include "log/manage_logger.hpp"
#include "common/util/screen_util.hpp"
#include "common/network/danejoe_serializer.hpp"
#include "client/connect/message_handler.hpp"

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
    connect(m_info_ok_button, &QPushButton::clicked, this, &FileInfoDialog::on_info_ok_button_clicked);
    connect(m_waiting_cancel_button, &QPushButton::clicked, this, &FileInfoDialog::on_waiting_cancel_button_clicked);
    m_is_init = true;
    connect(m_info_cancel_button, &QPushButton::clicked, this, &FileInfoDialog::on_info_cancel_button_clicked);
    connect(m_info_saved_path_button, &QPushButton::clicked, this, &FileInfoDialog::on_info_saved_path_button_clicked);
}

void FileInfoDialog::on_received_raw_file_info(std::vector<uint8_t> info)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    /// @todo 这个地方需要接受的是消息体
    auto file_info_opt = Client::MessageHandler::parse_download_response(info);
    if (!file_info_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "Failed to parse file info");
    }
    m_info_received =file_info_opt.value();
    QString file_info_text = QString(R"(
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
                <p><strong>文件名:</strong> <span id="file_name">%2</span></p>
                <p><strong>文件大小:</strong> <span id="file_size">%3</span></p>
                <p><strong>MD5码:</strong> <span        id="md5_code">%4</span></p>
            </div>

        </body>
        </html>
        )")
        .arg(m_info_received.file_id)
        // 默认saved_name为文件名
        .arg(QString::fromStdString(m_info_received.saved_name))
        .arg(m_info_received.file_size)
        .arg(QString::fromStdString(m_info_received.md5_code));

    /// @todo 使用富文本展示文件信息
    m_info_text_browser->setText(file_info_text);
    // 切换到文件信息界面
    m_stack_widget->setCurrentIndex(1);
}

void FileInfoDialog::on_waiting_cancel_button_clicked()
{
    /// @todo 取消传输
    this->hide();
}

void FileInfoDialog::set_operation(Operation operation)
{
    m_operation = operation;
}

void FileInfoDialog::on_info_ok_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    /// @todo 修改数据传入方式，添加ClientFileInfo参数
    QString saved_full_path = m_info_saved_path_line_edit->text();
    if (saved_full_path.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Saved path cannot be empty");
        return;
    }
    QFileInfo saved_full_path_info(saved_full_path);
    QString saved_path = saved_full_path_info.absoluteFilePath();
    QString saved_name = saved_full_path_info.fileName();
    m_info_received.saved_name = saved_name.toStdString();
    m_info_received.saved_path = saved_path.toStdString();
    m_info_received.operation = m_operation;
    this->hide();
    emit info_ok_button_clicked(m_info_received);
    m_info_received = ClientFileInfo();

}

void FileInfoDialog::on_info_cancel_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "FileInfoDialog", "FileInfoDialog has not been initialized");
        return;
    }
    m_info_received = ClientFileInfo();
    this->hide();
}

void FileInfoDialog::on_info_saved_path_button_clicked()
{
    QString saved_path = QFileDialog::getSaveFileName(this, "Save File", "/home/danejoe001/personal_code/code_cpp_project/cpp_project_trans/resource/client/download", "All Files (*)");
    m_info_saved_path_line_edit->setText(saved_path);
}
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCryptographicHash>

#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/database/sql_database_manager.hpp>

#include "server/view/add_file_dialog.hpp"
#include "server/model/server_file_info.hpp"
#include "server/model/server_file_info_table_model.hpp"
#include "common/util/screen_util.hpp"

AddFileDialog::AddFileDialog(QWidget* parent) :QDialog(parent)
{

}
void AddFileDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "AddFileDialog", "AddFileDialog has already been initialized");
        return;
    }
    this->setWindowTitle("Add File");
    ScreenUtil::RectInfo screen_rect = { 450, 250, 400, 100 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);
        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    DANEJOE_LOG_TRACE("default", "AddFileDialog", "Window rect: {}", screen_rect.to_string());
    m_file_dialog = new QFileDialog(this);
    m_file_path_layout = new QHBoxLayout(this);

    m_file_path_label = new QLabel("File Path:", this);
    m_file_path_line_edit = new QLineEdit(this);
    m_file_path_button = new QPushButton("...", this);
    m_add_file_button = new QPushButton("Add File", this);

    m_file_path_layout->addWidget(m_file_path_label);
    m_file_path_layout->addWidget(m_file_path_line_edit);
    m_file_path_layout->addWidget(m_file_path_button);
    m_file_path_layout->addWidget(m_add_file_button);
    m_file_path_layout->setStretch(0, 1);
    m_file_path_layout->setStretch(1, 7);
    m_file_path_layout->setStretch(2, 1);
    m_file_path_layout->setStretch(3, 1);

    m_file_info_service.init();

    connect(m_file_path_button, &QPushButton::clicked, this, &AddFileDialog::on_file_path_button_clicked);
    connect(m_add_file_button, &QPushButton::clicked, this, &AddFileDialog::on_add_file_button_clicked);
    m_is_init = true;
}

void AddFileDialog::on_file_path_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "AddFileDialog", "AddFileDialog has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "AddFileDialog", "on_file_path_button_clicked");
    QString file_path = m_file_dialog->getOpenFileName();
    m_file_path_line_edit->setText(file_path);
}

void AddFileDialog::on_add_file_button_clicked()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "AddFileDialog", "AddFileDialog has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "AddFileDialog", "on_add_file_button_clicked");
    QString file_path = m_file_path_line_edit->text();
    if (!QFile::exists(file_path))
    {
        QMessageBox::warning(this, "Error", "File does not exist");
        return;
    }
    QFileInfo qfile_info(file_path);
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "File cannot be opened");
        return;
    }
    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(&file))
    {
        QMessageBox::warning(this, "Error", "File cannot be hashed");
        return;
    }
    QString md5 = hash.result().toHex();
    ServerFileInfo file_info = ServerFileInfo(0, qfile_info.fileName().toStdString(), qfile_info.filePath().toStdString(), static_cast<uint32_t>(qfile_info.size()), md5.toStdString());

    bool result = m_file_info_service.add(file_info);
    file_info.file_id = m_file_info_service.count() + 1;
    if (result)
    {
        ServerFileInfoTableModel::get_instance()->add(file_info);
        QMessageBox::information(this, "Success", "File added successfully");
    }
}
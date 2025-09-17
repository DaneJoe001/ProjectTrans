#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCryptographicHash>

#include "server/view/add_file_dialog.hpp"
#include "log/manage_logger.hpp"
#include "server/model/server_file_info.hpp"
#include "common/database/database_manager.hpp"
#include "server/model/server_file_info_table_model.hpp"

AddFileDialog::AddFileDialog(QWidget* parent) :QDialog(parent)
{

}
void AddFileDialog::init()
{
    this->setWindowTitle("Add File");
    this->setGeometry(300, 300, 400, 100);
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
}

void AddFileDialog::on_file_path_button_clicked()
{
    DANEJOE_LOG_TRACE("default", "AddFileDialog", "on_file_path_button_clicked");
    QString file_path = m_file_dialog->getOpenFileName();
    m_file_path_line_edit->setText(file_path);
}

void AddFileDialog::on_add_file_button_clicked()
{
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
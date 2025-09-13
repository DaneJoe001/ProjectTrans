#pragma once

#include <QDialog>

#include "server/service/server_file_info_service.hpp"

class QLabel;
class QLineEdit;
class QPushButton;
class QFileDialog;
class QHBoxLayout;

class AddFileDialog : public QDialog
{
    Q_OBJECT
public:
    AddFileDialog(QWidget* parent = nullptr);
    void init();
public slots:
    void on_file_path_button_clicked();
    void on_add_file_button_clicked();
private:
    QLabel* m_file_path_label;
    QLineEdit* m_file_path_line_edit;
    QPushButton* m_file_path_button;
    QPushButton* m_add_file_button;
    QFileDialog* m_file_dialog;
    QHBoxLayout* m_file_path_layout;
    ServerFileInfoService m_file_info_service;
};
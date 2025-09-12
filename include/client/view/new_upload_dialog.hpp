#pragma once

#include <QDialog>

class QPushButton;
class QLineEdit;
class QLabel;
class QTextBrowser;
class QProgressBar;
class QHBoxLayout;
class QVBoxLayout;
class QFileDialog;

class NewUploadDialog : public QDialog
{
    Q_OBJECT
public:
    NewUploadDialog(QWidget* parent = nullptr);
    void init();
private:
    QVBoxLayout* m_main_layout = nullptr;

    QFileDialog* m_file_dialog = nullptr;

    QWidget* m_file_path_widget = nullptr;
    QHBoxLayout* m_file_path_layout = nullptr;
    QLabel* m_file_path_label = nullptr;
    QLineEdit* m_file_path_line_edit = nullptr;
    QPushButton* m_path_select_push_button = nullptr;
    QPushButton* m_add_file_push_button = nullptr;

    QLabel* m_url_label = nullptr;
    QLineEdit* m_url_line_edit = nullptr;
    QPushButton* m_upload_push_button = nullptr;
    QHBoxLayout* m_url_layout = nullptr;
    QWidget* m_url_widget = nullptr;

    QLabel* m_username_label = nullptr;
    QLineEdit* m_username_line_edit = nullptr;
    QLabel* m_password_label = nullptr;
    QLineEdit* m_password_line_edit = nullptr;
    QHBoxLayout* m_user_info_layout = nullptr;
    QWidget* m_user_info_widget = nullptr;

    QProgressBar* m_upload_progress_bar = nullptr;
    QTextBrowser* m_upload_info_browser = nullptr;
};
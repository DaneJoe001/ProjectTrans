#pragma once

#include <QDialog>

class QPushButton;
class QLineEdit;
class QLabel;
class QTextBrowser;
class QProgressBar;
class QHBoxLayout;
class QVBoxLayout;
class TransManager;

class NewDownloadDialog : public QDialog
{
    Q_OBJECT
public:
    NewDownloadDialog(QWidget* parent = nullptr);
    void init();
public slots:
    void on_download_push_button_clicked();
private:
    QVBoxLayout* m_main_layout = nullptr;

    QLabel* m_url_label = nullptr;
    QLineEdit* m_url_line_edit = nullptr;
    QPushButton* m_download_push_button = nullptr;
    QHBoxLayout* m_url_layout = nullptr;
    QWidget* m_url_widget = nullptr;

    QLabel* m_username_label = nullptr;
    QLineEdit* m_username_line_edit = nullptr;
    QLabel* m_password_label = nullptr;
    QLineEdit* m_password_line_edit = nullptr;
    QHBoxLayout* m_user_info_layout = nullptr;
    QWidget* m_user_info_widget = nullptr;

    QProgressBar* m_download_progress_bar = nullptr;
    QTextBrowser* m_download_info_browser = nullptr;

    TransManager* m_trans_manager = nullptr;

};
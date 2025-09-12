#pragma once

#include <cstdint>
#include <vector>

#include <QMainWindow>
#include <QString>

/// @todo 创建一个客户端管理器，分配管理多个客户端实例

class QMenuBar;
class QMenu;
class QAction;
class QMenuBar;
class ConnectionTestDialog;
class TransDialog;
class NewDownloadDialog;
class NewUploadDialog;

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    ClientMainWindow(QWidget* parent = nullptr);
    ~ClientMainWindow();
    void init();
signals:
    /**
     * @brief 数据接收完成信号，定时器循环调用，接收数据
     * @param data 接收到的数据
     */
    void message_received(const std::vector<uint8_t>& data);
private:
    void timerEvent(QTimerEvent* event)override;
public slots:
    void on_connection_test_action_triggered();
    void on_connection_test_dialog_closed();
    void on_new_upload_action_triggered();
    void on_new_download_action_triggered();
private:
    bool m_is_on_connection_test = false;

    QString m_window_title = "Client";

    QMenuBar* m_menubar = nullptr;

    QMenu* m_task_menu = nullptr;
    QAction* m_start_task_action = nullptr;
    QAction* m_stop_task_action = nullptr;
    QAction* m_setting_action = nullptr;
    QAction* m_connection_test_action = nullptr;

    QMenu* m_download_menu = nullptr;
    QAction* m_new_download_action = nullptr;

    QMenu* m_upload_menu = nullptr;
    QAction* m_new_upload_action = nullptr;

    QMenu* m_view_menu = nullptr;
    QAction* m_sort_action = nullptr;

    ConnectionTestDialog* m_connection_test_dialog = nullptr;
    TransDialog* m_trans_dialog = nullptr;
    NewDownloadDialog* m_new_download_dialog = nullptr;
    NewUploadDialog* m_new_upload_dialog = nullptr;
};
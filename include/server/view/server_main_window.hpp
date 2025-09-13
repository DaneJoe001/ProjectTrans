#pragma once

#include <QMainWindow>

class QStackedWidget;
class QMenuBar;
class QMenu;
class QAction;
class AddFileDialog;
class ResourceInfoWidget;
class ConnectionInfoWidget;

class ServerMainWindow : public QMainWindow {
public:
    Q_OBJECT
public:
    ServerMainWindow(QWidget* parent = nullptr);
    void init();
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
public slots:
    void on_add_file_action_triggered();
    void on_remove_file_action_triggered();
    void on_resource_info_view_action_triggered();
    void on_connection_info_view_action_triggered();
private:
    QStackedWidget* m_stacked_widget = nullptr;

    QMenuBar* m_menu_bar = nullptr;

    QMenu* m_file_menu = nullptr;
    QAction* m_add_file_action = nullptr;
    QAction* m_remove_file_action = nullptr;

    QMenu* m_view_menu = nullptr;
    QAction* m_resource_info_view_action = nullptr;
    QAction* m_connection_info_view_action = nullptr;

    AddFileDialog* m_add_file_dialog = nullptr;

    ResourceInfoWidget* m_resource_info_widget = nullptr;
    ConnectionInfoWidget* m_connection_info_widget = nullptr;
};
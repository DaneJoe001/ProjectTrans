#include <atomic>

#include <QCloseEvent>
#include <QKeyEvent>
#include <QAction>
#include <QStackedWidget>
#include <QTimerEvent>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "server/view/server_main_window.hpp"
#include "server/view/add_file_dialog.hpp"
#include "server/view/connection_info_widget.hpp"
#include "server/view/resource_info_widget.hpp"
#include "log/manage_logger.hpp"

extern std::atomic<bool> g_is_panel_running;

ServerMainWindow::ServerMainWindow(QWidget* parent) :QMainWindow(parent) {}
void ServerMainWindow::init()
{
    this->setGeometry(200, 200, 800, 600);
    this->setWindowTitle("Server Panel");
    m_stacked_widget = new QStackedWidget(this);

    m_stacked_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setCentralWidget(m_stacked_widget);

    m_resource_info_widget = new ResourceInfoWidget(this);
    m_stacked_widget->addWidget(m_resource_info_widget);
    m_resource_info_widget->init();
    m_connection_info_widget = new ConnectionInfoWidget(this);
    m_stacked_widget->addWidget(m_connection_info_widget);
    m_connection_info_widget->init();

    m_stacked_widget->setCurrentIndex(0);

    m_menu_bar = new QMenuBar(this);
    this->setMenuBar(m_menu_bar);
    m_file_menu = m_menu_bar->addMenu("File");
    m_add_file_action = m_file_menu->addAction("Add File");
    m_remove_file_action = m_file_menu->addAction("Remove File");

    m_view_menu = m_menu_bar->addMenu("View");
    m_resource_info_view_action = m_view_menu->addAction("Resource Info");
    m_connection_info_view_action = m_view_menu->addAction("Connection Info");

    m_add_file_dialog = new AddFileDialog(this);
    m_add_file_dialog->init();
    this->setStyleSheet(
        "#connection_info_widget { background-color: #713535; } "
        "#resource_info_widget { background-color: #7561f4; }"
    );


    connect(m_add_file_action, &QAction::triggered, this, &ServerMainWindow::on_add_file_action_triggered);
    connect(m_remove_file_action, &QAction::triggered, this, &ServerMainWindow::on_remove_file_action_triggered);
    connect(m_resource_info_view_action, &QAction::triggered, this, &ServerMainWindow::on_resource_info_view_action_triggered);
    connect(m_connection_info_view_action, &QAction::triggered, this, &ServerMainWindow::on_connection_info_view_action_triggered);

    startTimer(500);
}

void ServerMainWindow::closeEvent(QCloseEvent* event)
{
    event->ignore(); // 忽略关闭事件
    hide(); // 隐藏窗口
    g_is_panel_running.store(false);
}

void ServerMainWindow::timerEvent(QTimerEvent* event)
{
    if (g_is_panel_running.load() && !this->isVisible())
    {
        this->show();
    }
}

void ServerMainWindow::on_add_file_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_add_file_action_triggered");
    m_add_file_dialog->show();
}

void ServerMainWindow::on_remove_file_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_remove_file_action_triggered");
}
void ServerMainWindow::on_resource_info_view_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_resource_info_view_action_triggered");
    m_stacked_widget->setCurrentIndex(0);
}
void ServerMainWindow::on_connection_info_view_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_connection_info_view_action_triggered");
    m_stacked_widget->setCurrentIndex(1);
}
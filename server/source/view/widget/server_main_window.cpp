#include <atomic>

#include <QCloseEvent>
#include <QKeyEvent>
#include <QAction>
#include <QStackedWidget>
#include <QTimerEvent>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <danejoe/logger/logger_manager.hpp>
#include <qmainwindow.h>

#include "common/util/screen_util.hpp"
#include "view/widget/server_main_window.hpp"
#include "view/dialog/add_file_dialog.hpp"
#include "view/widget/connection_info_widget.hpp"
#include "view/widget/resource_info_widget.hpp"

ServerMainWindow::ServerMainWindow(QWidget* parent) :QMainWindow(parent) {}
void ServerMainWindow::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ServerMainWindow", "ServerMainWindow has been initialized");
        return;
    }
    auto screen_rect = ScreenUtil::get_screen_rect(1);
    ScreenUtil::RectInfo window_rect = { -1, -1, 800, 600 };
    auto point = ScreenUtil::get_destination_point(screen_rect, window_rect, ScreenUtil::RealativePosition::Center);
    setGeometry(point.x, point.y, 800, 600);
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
    m_is_init = true;
}

void ServerMainWindow::on_add_file_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ServerMainWindow", "ServerMainWindow has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_add_file_action_triggered");
    m_add_file_dialog->show();
}

void ServerMainWindow::on_remove_file_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ServerMainWindow", "ServerMainWindow has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_remove_file_action_triggered");
}
void ServerMainWindow::on_resource_info_view_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ServerMainWindow", "ServerMainWindow has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_resource_info_view_action_triggered");
    m_stacked_widget->setCurrentIndex(0);
}
void ServerMainWindow::on_connection_info_view_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "ServerMainWindow", "ServerMainWindow has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "ServerMainWindow", "on_connection_info_view_action_triggered");
    m_stacked_widget->setCurrentIndex(1);
}
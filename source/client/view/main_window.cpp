#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTableWidget>
#include <QString>

#include "client/view/main_window.hpp"
#include "log/manage_logger.hpp"
#include "client/view/connection_test_dialog.hpp"
#include "client/view/new_download_dialog.hpp"
#include "client/view/new_upload_dialog.hpp"
#include "client/view/trans_dialog.hpp"

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent) {}

void MainWindow::init()
{
    setWindowTitle(m_window_title);
    setGeometry(400, 200, 800, 600);
    /// @brief 菜单栏
    m_menubar = new QMenuBar(this);
    setMenuBar(m_menubar);
    /// @brief 任务菜单
    m_task_menu = new QMenu(this);
    m_task_menu->setTitle("Task");
    m_menubar->addMenu(m_task_menu);

    m_start_task_action = new QAction(this);
    m_start_task_action->setText("Start Task");
    m_task_menu->addAction(m_start_task_action);
    m_stop_task_action = new QAction(this);
    m_stop_task_action->setText("Stop Task");
    m_task_menu->addAction(m_stop_task_action);
    m_setting_action = new QAction(this);
    m_setting_action->setText("Setting");
    m_task_menu->addAction(m_setting_action);
    m_connection_test_action = new QAction(this);
    m_connection_test_action->setText("Connection Test");
    m_task_menu->addAction(m_connection_test_action);


    /// @brief 下载菜单
    m_download_menu = new QMenu(this);
    m_download_menu->setTitle("Download");
    m_menubar->addMenu(m_download_menu);

    m_new_download_action = new QAction(this);
    m_new_download_action->setText("New Download");
    m_download_menu->addAction(m_new_download_action);

    /// @brief 上传菜单
    m_upload_menu = new QMenu(this);
    m_upload_menu->setTitle("Upload");
    m_menubar->addMenu(m_upload_menu);

    m_new_upload_action = new QAction(this);
    m_new_upload_action->setText("New Upload");
    m_upload_menu->addAction(m_new_upload_action);

    /// @brief 视图菜单
    m_view_menu = new QMenu(this);
    m_view_menu->setTitle("View");
    m_menubar->addMenu(m_view_menu);

    m_sort_action = new QAction(this);
    m_sort_action->setText("Sort");
    m_view_menu->addAction(m_sort_action);

    /// @brief 连接测试对话框
    m_connection_test_dialog = new ConnectionTestDialog(this);
    /// @brief 新任务对话框
    m_new_download_dialog = new NewDownloadDialog(this);
    m_new_download_dialog->init();
    m_new_upload_dialog = new NewUploadDialog(this);
    m_new_upload_dialog->init();
    /// @brief 传输对话框
    m_trans_dialog = new TransDialog(this);

    m_connection_test_dialog->init();

    connect(m_connection_test_action, &QAction::triggered, this, &MainWindow::on_connection_test_action_triggered);
    connect(m_new_download_action, &QAction::triggered, this, &MainWindow::on_new_download_action_triggered);
    connect(m_new_upload_action, &QAction::triggered, this, &MainWindow::on_new_upload_action_triggered);

    connect(m_connection_test_dialog, &QDialog::finished, this, &MainWindow::on_connection_test_dialog_closed);

    startTimer(1000);
}

void MainWindow::timerEvent(QTimerEvent* event)
{

}

MainWindow::~MainWindow()
{

}

void MainWindow::on_connection_test_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_action_triggered");
    m_connection_test_dialog->show();
    m_is_on_connection_test = true;
}

void MainWindow::on_connection_test_dialog_closed()
{
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_dialog_closed");
    m_is_on_connection_test = false;
}

void MainWindow::on_new_upload_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_new_upload_action_triggered");
    m_new_upload_dialog->show();
}

void MainWindow::on_new_download_action_triggered()
{
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_new_download_action_triggered");
    m_new_download_dialog->show();
}
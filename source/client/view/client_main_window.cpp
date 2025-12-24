#include <QMenuBar>
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QStackedWidget>
#include <QString>
#include <QScreen>
#include <QMessageBox>

#include "client/view/client_main_window.hpp"
#include <danejoe/logger/logger_manager.hpp>
#include "client/view/connection_test_dialog.hpp"
#include "client/view/new_download_dialog.hpp"
#include "client/view/new_upload_dialog.hpp"
#include "client/view/trans_dialog.hpp"
#include "client/view/file_trans_info_widget.hpp"
#include "common/util/screen_util.hpp"
#include "client/model/file_trans_info_table_model.hpp"
#include "client/connect/trans_manager.hpp"

ClientMainWindow::ClientMainWindow(QWidget* parent) :QMainWindow(parent) {}

void ClientMainWindow::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "MainWindow", "init", "client main window has been initialized");
        return;
    }
    // 设置客户端窗口标题
    setWindowTitle(m_window_title);
    // 设置客户端窗口尺寸位置
    auto screen_rect = ScreenUtil::get_screen_rect(1);
    ScreenUtil::RectInfo window_rect = { -1, -1, 800, 600 };
    auto point = ScreenUtil::get_destination_point(screen_rect, window_rect, ScreenUtil::RealativePosition::Center);
    setGeometry(point.x, point.y, 800, 600);
    // 初始化菜单栏
    m_menubar = new QMenuBar(this);
    // 设置当前菜单栏
    setMenuBar(m_menubar);
    // 初始化任务菜单
    m_task_menu = new QMenu(this);
    // 设置任务菜单标题
    m_task_menu->setTitle("Task");
    // 添加任务菜单
    m_menubar->addMenu(m_task_menu);

    // 初始化开始任务行为
    m_start_task_action = new QAction(this);
    // 设置开始任务行为标题
    m_start_task_action->setText("Start Task");
    // 添加开始任务行为
    m_task_menu->addAction(m_start_task_action);
    // 初始化停止任务行为
    m_stop_task_action = new QAction(this);
    // 设置停止任务行为标题
    m_stop_task_action->setText("Stop Task");
    // 添加停止任务行为
    m_task_menu->addAction(m_stop_task_action);
    // 初始化设置行为
    m_setting_action = new QAction(this);
    // 设置设置行为标题
    m_setting_action->setText("Setting");
    // 添加设置行为
    m_task_menu->addAction(m_setting_action);
    // 添加连接测试行为
    m_connection_test_action = new QAction(this);
    // 设置连接测试行为标题
    m_connection_test_action->setText("Connection Test");
    // 添加连接测试行为
    m_task_menu->addAction(m_connection_test_action);

    // 初始化下载菜单
    m_download_menu = new QMenu(this);
    // 设置下载菜单标题
    m_download_menu->setTitle("Download");
    // 添加下载菜单
    m_menubar->addMenu(m_download_menu);

    // 添加新建下载行为
    m_new_download_action = new QAction(this);
    // 设置下载行为标题
    m_new_download_action->setText("New Download");
    // 添加新建下载行为
    m_download_menu->addAction(m_new_download_action);

    // 初始化上传菜单
    m_upload_menu = new QMenu(this);
    // 设置上传菜单标题
    m_upload_menu->setTitle("Upload");
    // 添加新建上传菜单
    m_menubar->addMenu(m_upload_menu);

    // 初始化新建上传行为
    m_new_upload_action = new QAction(this);
    // 设置新建上传行为标题
    m_new_upload_action->setText("New Upload");
    // 添加新建上传行为
    m_upload_menu->addAction(m_new_upload_action);

    // 视图菜单
    m_view_menu = new QMenu(this);
    // 设置视图菜单标题
    m_view_menu->setTitle("View");
    // 添加视图菜单
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

    /// @brief 主界面堆栈窗口
    m_stack_widget = new QStackedWidget(this);
    this->setCentralWidget(m_stack_widget);
    m_file_trans_info_widget = new FileTransInfoWidget(this);
    m_file_trans_info_widget->init();
    m_stack_widget->addWidget(m_file_trans_info_widget);
    m_stack_widget->setCurrentIndex(0);


    //连接测试行为
    connect(m_connection_test_action, &QAction::triggered, this, &ClientMainWindow::on_connection_test_action_triggered);
    connect(m_new_download_action, &QAction::triggered, this, &ClientMainWindow::on_new_download_action_triggered);
    connect(m_new_upload_action, &QAction::triggered, this, &ClientMainWindow::on_new_upload_action_triggered);
    connect(m_connection_test_dialog, &QDialog::finished, this, &ClientMainWindow::on_connection_test_dialog_closed);
    connect(m_file_trans_info_widget, &FileTransInfoWidget::row_clicked, this, &ClientMainWindow::on_file_trans_selected);
    connect(m_start_task_action, &QAction::triggered, this, &ClientMainWindow::on_start_task_action_triggered);
    connect(m_stop_task_action, &QAction::triggered, this, &ClientMainWindow::on_stop_task_action_triggered);

    startTimer(1000);
    m_is_init = true;
}

void ClientMainWindow::timerEvent(QTimerEvent* event)
{

}

ClientMainWindow::~ClientMainWindow()
{

}

void ClientMainWindow::on_connection_test_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_action_triggered");
    m_connection_test_dialog->show();
    m_is_on_connection_test = true;
}

void ClientMainWindow::on_connection_test_dialog_closed()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_dialog_closed");
    m_is_on_connection_test = false;
}

void ClientMainWindow::on_new_upload_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_new_upload_action_triggered");
    m_new_upload_dialog->show();
}

void ClientMainWindow::on_new_download_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_new_download_action_triggered");
    m_new_download_dialog->show();
}

void ClientMainWindow::on_file_trans_selected(int32_t row)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_file_trans_selected");
    if (row < 0)
    {
        return;
    }
    m_selected_task_index = row;
}

void ClientMainWindow::on_start_task_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    if (m_selected_task_index < 0)
    {
        QMessageBox::warning(this, "Warning", "Currently, no tasks have been selected.");
        return;
    }
    auto file_info = FileTransInfoTableModel::get_instance()->get_file_info(m_selected_task_index);
    if (file_info.file_id < 0)
    {
        QMessageBox::warning(this, "Warning", "Invalid task selected.");
        return;
    }
    /// @todo 处理调用点
    // m_trans_manager->add_trans(file_info);
    DANEJOE_LOG_TRACE("default", "MainWindow", "Task file info: {}", file_info.to_string());
    QMessageBox::information(this, "info", "Task will start soon.");
}

void ClientMainWindow::on_stop_task_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    if (m_selected_task_index < 0)
    {
        QMessageBox::warning(this, "Warning", "Currently, no tasks have been selected.");
        return;
    }
    auto file_info = FileTransInfoTableModel::get_instance()->get_file_info(m_selected_task_index);
    if (file_info.file_id < 0)
    {
        QMessageBox::warning(this, "Warning", "Invalid task selected.");
        return;
    }

    DANEJOE_LOG_TRACE("default", "MainWindow", "Task file info: {}", file_info.to_string());
    QMessageBox::information(this, "info", "Task will stop soon.");
}

void ClientMainWindow::on_view_update()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    this->update();
}
#include <QMenuBar>
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QStackedWidget>
#include <QString>
#include <QScreen>
#include <QMessageBox>

#include "danejoe/logger/logger_manager.hpp"

#include "common/util/screen_util.hpp"
#include "view/dialog/connection_test_dialog.hpp"
#include "view/dialog/new_download_dialog.hpp"
#include "view/dialog/new_upload_dialog.hpp"
#include "view/dialog/task_info_dialog.hpp"
#include "view/widget/task_table_widget.hpp"
#include "view/widget/client_main_window.hpp"

ClientMainWindow::ClientMainWindow(
    TaskService& task_service,
    ClientFileService& client_file_service,
    BlockService& block_service,
    QPointer<ViewEventHub> view_event_hub,
    QPointer<BlockScheduleController> block_schedule_controller,
    QWidget* parent) :
    QMainWindow(parent),
    m_task_service(task_service),
    m_client_file_service(client_file_service),
    m_block_service(block_service),
    m_view_event_hub(view_event_hub),
    m_block_schedule_controller(block_schedule_controller)
{}

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
    m_connection_test_dialog = new ConnectionTestDialog(m_view_event_hub, this);
    /// @brief 新任务对话框
    m_new_download_dialog = new NewDownloadDialog(m_task_service, m_client_file_service, m_block_service, m_view_event_hub, this);
    m_new_download_dialog->init();
    m_new_upload_dialog = new NewUploadDialog(this);
    m_new_upload_dialog->init();

    m_connection_test_dialog->init();

    m_task_table_model = new TaskTableModel (m_task_service, this);

    /// @brief 主界面堆栈窗口
    m_stack_widget = new QStackedWidget(this);
    this->setCentralWidget(m_stack_widget);
    m_task_table_widget = new TaskTableWidget(m_task_service, m_view_event_hub, m_task_table_model, this);
    m_task_table_widget->init();
    m_stack_widget->addWidget(m_task_table_widget);
    m_stack_widget->setCurrentIndex(0);


    //连接测试行为
    connect(m_connection_test_action, &QAction::triggered, this, &ClientMainWindow::on_connection_test_action_triggered);
    connect(m_new_download_action, &QAction::triggered, this, &ClientMainWindow::on_new_download_action_triggered);
    connect(m_new_upload_action, &QAction::triggered, this, &ClientMainWindow::on_new_upload_action_triggered);
    connect(m_connection_test_dialog, &QDialog::finished, this, &ClientMainWindow::on_connection_test_dialog_closed);
    connect(m_task_table_widget, &TaskTableWidget::task_selected, this, &ClientMainWindow::on_task_selected);
    connect(m_start_task_action, &QAction::triggered, this, &ClientMainWindow::on_start_task_action_triggered);
    connect(m_stop_task_action, &QAction::triggered, this, &ClientMainWindow::on_stop_task_action_triggered);
    connect(this, &ClientMainWindow::task_enqueue, m_block_schedule_controller, &BlockScheduleController::on_task_enqueue);
    m_is_init = true;
    connect(m_block_schedule_controller, &BlockScheduleController::task_completed, this, &ClientMainWindow::on_task_completed);
}

ClientMainWindow::~ClientMainWindow() {}

void ClientMainWindow::on_connection_test_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_action_triggered");
    m_connection_test_dialog->show();
}

void ClientMainWindow::on_connection_test_dialog_closed()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "on_connection_test_dialog_closed");
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

void ClientMainWindow::on_task_selected(int64_t task_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    DANEJOE_LOG_TRACE("default", "MainWindow", "Task selected: {}", task_id);
    if (task_id < 0)
    {
        DANEJOE_LOG_WARN("default", "ClientMainWindow", "Invalid task id");
        return;
    }
    m_selected_task_id = task_id;
}

void ClientMainWindow::on_start_task_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    if (m_selected_task_id < 0)
    {
        DANEJOE_LOG_WARN("default", "ClientMainWindow", "Have not select task!");
        QMessageBox::warning(this, "warn", "Have not select task!");
        return;
    }
    auto task_entity_opt = m_task_service.get_by_task_id(m_selected_task_id);
    if (!task_entity_opt.has_value())
    {
        DANEJOE_LOG_WARN("default", "ClientMainWindow", "No mathced task");
        QMessageBox::warning(this, "warn", "Task is not exist!");
        return;
    }
    EventContext event_context;
    event_context.m_object_name = "ClientMainWindow";
    NetworkEndpoint endpoint;
    auto url_info = m_resolver.parse(task_entity_opt->source_url);
    endpoint.ip = url_info.host;
    endpoint.port = url_info.port;
    emit task_enqueue(event_context, endpoint, task_entity_opt.value());
    QMessageBox::information(this, "info", QString("Task %1 will start soon.").arg(m_selected_task_id));
}

void ClientMainWindow::on_stop_task_action_triggered()
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }

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

void ClientMainWindow::on_task_completed(int64_t task_id)
{
    if (!m_is_init)
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init", "client main window has not been initialized");
        return;
    }
    m_task_table_widget->on_task_update(task_id);
    QMessageBox::information(this, "Info", QString("Task %1 is finished!").arg(task_id));
}
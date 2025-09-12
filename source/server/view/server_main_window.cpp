#include <atomic>

#include <QCloseEvent>
#include <QKeyEvent>
#include <QAction>

#include "server/view/server_main_window.hpp"

extern std::atomic<bool> g_is_panel_running;

ServerMainWindow::ServerMainWindow(QWidget* parent) :QMainWindow(parent) {}
void ServerMainWindow::init()
{
    this->setGeometry(200, 200, 800, 600);
    this->setWindowTitle("Server Panel");
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
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <QApplication>

#include "log/manage_logger.hpp"
#include "server/main/server.hpp"
#include "server/view/server_main_window.hpp"

using namespace std::literals::chrono_literals;
int32_t main(int32_t argc, char** argv)
{
    set_logger();
    QApplication app(argc, argv);

    DANEJOE_LOG_TRACE("default", "Server", "Hello,world!");
    // 设置退出信号
    std::signal(SIGINT, stop_server_handler);
    // 设置启动界面信号
    std::signal(SIGQUIT, open_window_handler);
    // 清理数据库
    // clear_database();
    // 初始化数据库
    init_database();
    // 清理日志
    clear_log();
    ServerMainWindow window;
    window.init();
    window.show();
    std::jthread run_server_thread(run_server);
    return app.exec();
}
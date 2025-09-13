#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <QApplication>

#include "log/manage_logger.hpp"
#include "server/main/server.hpp"
#include "server/view/server_main_window.hpp"

using namespace std::literals::chrono_literals;
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    DANEJOE_LOG_TRACE("default", "Server", "Hello,world!");
    std::signal(SIGINT, stop_server_handler);
    std::signal(SIGQUIT, open_window_handler);
    // clear_database();
    init_database();
    clear_log();
    ServerMainWindow window;
    window.init();
    window.show();
    std::jthread run_server_thread(run_server);
    return app.exec();
}
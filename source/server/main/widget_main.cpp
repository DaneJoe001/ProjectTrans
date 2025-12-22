#include <QApplication>

#include <danejoe/logger/logger_manager.hpp>

#include "server/main/server_app.hpp"

using namespace std::literals::chrono_literals;
int main(int32_t argc, char** argv)
{
    QApplication app(argc, argv);
    ServerApp server_app;
    app.setQuitOnLastWindowClosed(false);
    app.installEventFilter(&server_app);
    server_app.init();
    server_app.show_main_window();

    return app.exec();
}
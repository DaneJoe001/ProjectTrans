#include <QApplication>

#include <danejoe/logger/logger_manager.hpp>

#include "client/view/client_main_window.hpp"
#include "client/main/client_app.hpp"

#include "client/view/connection_test_dialog.hpp"


int32_t main(int32_t argc, char** argv)
{
    QApplication app(argc, argv);
    // ClientApp client_app;
    // client_app.init();
    // client_app.show_main_window();
    ConnectionTestDialog connection_test_dialog;
    connection_test_dialog.init();
    connection_test_dialog.show();
    return app.exec();
}
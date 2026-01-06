#include <QApplication>

#include <cstdlib>
#include <exception>

#include <danejoe/logger/logger_manager.hpp>

#include "main/server_app.hpp"

using namespace std::literals::chrono_literals;

namespace
{
    void terminate_handler()
    {
        try
        {
            auto eptr = std::current_exception();
            if (eptr)
            {
                std::rethrow_exception(eptr);
            }
            DANEJOE_LOG_ERROR("default", "ServerMain", "std::terminate called with no active exception");
        }
        catch (const std::exception& e)
        {
            DANEJOE_LOG_ERROR("default", "ServerMain", "std::terminate: {}", e.what());
        }
        catch (...)
        {
            DANEJOE_LOG_ERROR("default", "ServerMain", "std::terminate: unknown exception");
        }
        std::abort();
    }
}

int main(int32_t argc, char** argv)
{
    std::set_terminate(terminate_handler);

    QApplication app(argc, argv);
    ServerApp server_app;
    app.setQuitOnLastWindowClosed(false);
    app.installEventFilter(&server_app);
    server_app.init();
    server_app.show_main_window();

    const int ret = app.exec();
    DANEJOE_LOG_WARN("default", "ServerMain", "app.exec() returned: {}", ret);
    return ret;
}
#include <QApplication>
#include <iostream>
#include <thread>

#include "log/manage_logger.hpp"
#include "view/main_window.hpp"
#include "network/posix_client_socket.hpp"
#include "database/sqlite_database.hpp"

// #define CONSOLE_CLIENT

#ifndef CONSOLE_CLIENT
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    DANEJOE_LOG_TRACE("default", "Client", "Hello,world!");
    MainWindow window;
    window.init();
    window.show();
    return app.exec();
}
#else

int main(int argc, char** argv)
{
    PosixClientSocket socket("127.0.0.1", 8080);
    while (true)
    {
        std::string data;
        DANEJOE_LOG_INFO("default", "Client", "Enter data: ");
        std::cin >> data;
        socket.send_all(std::vector<uint8_t>(data.begin(), data.end()));
        while (true)
        {
            if (!socket.is_valid())
            {
                return 0;
            }
            auto recv_data = socket.read_all();
            if (recv_data.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            DANEJOE_LOG_TRACE("default", "Client", "Received:{} ", std::string(recv_data.begin(), recv_data.end()));
            break;
        }
    }
}

#endif
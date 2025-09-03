#include <QApplication>
#include <iostream>

#include "log/manage_logger.hpp"
#include "view/main_window.hpp"
#include  "network/posix_client_socket.hpp"

#define CONSOLE_CLIENT

#ifndef CONSOLE_CLIENT
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    DANEJOE_LOG_TRACE("default", "Client", "Hello,world!");
    MainWindow window;
    window.show();
    return app.exec();
}
#endif

int main(int argc, char** argv)
{
    PosixClientSocket socket("127.0.0.1", 8080);
    while (true)
    {
        std::string data;
        std::cout << "Enter data: ";
        std::cin >> data;
        socket.send(std::vector<uint8_t>(data.begin(), data.end()));
        auto recv_data = socket.recieve(5);
        std::cout << "Received: " << std::string(recv_data.begin(), recv_data.end()) << std::endl;
    }
}
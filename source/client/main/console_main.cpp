#include <QApplication>
#include <iostream>
#include <thread>

#include <danejoe/logger/logger_manager.hpp>

#include "danejoe/network/socket/posix_client_socket.hpp"

int main()
{
    DaneJoe::PosixClientSocket socket("127.0.0.1", 8080);
    socket.set_non_blocking(true);
    while (true)
    {
        std::string data;
        DANEJOE_LOG_INFO("default", "Client", "Enter data: ");
        std::cin >> data;
        socket.write_all(std::vector<uint8_t>(data.begin(), data.end()));
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
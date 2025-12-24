#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#include <QApplication>

#include <danejoe/logger/logger_manager.hpp>

#include "danejoe/network/socket/posix_client_socket.hpp"

#include "client/connect/message_handler.hpp"
#include "danejoe/network/codec/frame_assembler.hpp"

#include "client/service/trans_service.hpp"

int main(int argc, char* argv[])
{
    DANEJOE_LOG_DEBUG("default", "ConsoleApp", "Main start");
    QApplication app(argc, argv);
    TransService trans_service;
    Endpoint endpoint = { "127.0.0.1", 8080 };
    trans_service.send_test_request(endpoint, "Hello World");
    return app.exec();
}
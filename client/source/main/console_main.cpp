#include <string>

#include <QApplication>

#include <danejoe/logger/logger_manager.hpp>

#include "service/trans_service.hpp"

int main(int argc, char* argv[])
{
    DANEJOE_LOG_DEBUG("default", "ConsoleApp", "Main start");
    QApplication app(argc, argv);
    TransService& trans_service = TransService::get_instance();
    NetworkEndpoint endpoint = { "127.0.0.1", 8080 };
    TestRequestTransfer request;
    request.message = "Hello World";
    trans_service.send_test_request(endpoint, request);
    return app.exec();
}
#include "log/manage_logger.hpp"
#include "network/posix_server_socket.hpp"
#include "network/posix_client_socket.hpp"

int main(int argc, char** argv)
{
    DANEJOE_LOG_TRACE("default", "Server", "Hello,world!");

    /// @brief 设置地址复用，后续再抽象拓展
    int opt_val = 1;
    ISocket::IOption option;
    option.level = 1;
    option.opt_name = 2;
    option.opt_val = &opt_val;
    option.opt_len = sizeof(opt_val);

    PosixServerSocket server("0.0.0.0", 8080, option);

    auto client = server.accept();
    std::vector<uint8_t> data(5);
    while (true)
    {
        data = client->recieve(5);
        DANEJOE_LOG_INFO("default", "Server", "Recieved: {}", std::string(data.begin(), data.end()));
        client->send(data);
    }
    return 0;
}
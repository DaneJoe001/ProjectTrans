#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "network/i_client_socket.hpp"

class PosixClientSocket : public IClientSocket
{
public:
    struct PosixOption
    {
        int level;
        int opt_name;
        void* opt_val;
        socklen_t opt_len;
    };
public:
    PosixClientSocket(int socket);
    PosixClientSocket(const std::string& ip, uint16_t  port);
    ~PosixClientSocket();
    PosixOption to_posix_option(const IOption& option);
    void connect(const std::string& ip, uint16_t port) override;
    std::vector<uint8_t> recieve(std::size_t size) override;
    void send(const std::vector<uint8_t>& data) override;
    bool is_valid()const override;
    bool set_opt(const IOption& option)override;
    void close()override;
private:
    int m_socket;
};
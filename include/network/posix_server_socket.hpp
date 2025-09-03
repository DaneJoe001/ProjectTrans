#pragma once

#include "network/i_server_socket.hpp"

class PosixServerSocket : public IServerSocket
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
    PosixServerSocket(const std::string& ip, uint16_t  port, const IOption& option);
    ~PosixServerSocket();
    void listen()override;
    PosixOption to_posix_option(const IOption& option);
    std::unique_ptr<IClientSocket> accept()override;
    void bind(const std::string& ip, uint16_t port)override;
    bool is_valid()const override;
    bool set_opt(const IOption& option)override;
    void close()override;
private:
    int m_socket = -1;
};
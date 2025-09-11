#pragma once

#include "common/network/posix_client_socket.hpp"

class ClientConnection
{
public:
    ClientConnection(const std::string& ip, uint16_t port);
    ~ClientConnection();
    void send(const std::vector<uint8_t>& data);
    std::vector<uint8_t> recieve();
    bool is_same(const std::string& ip, uint16_t port);
    std::pair<std::string, uint16_t> get_id();
private:
    PosixClientSocket m_socket;
    std::string m_ip;
    uint16_t m_port;
};
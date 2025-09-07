#pragma once

#include <string>
#include <memory>
#include <list>
#include <cstdint>

#include "network/i_socket.hpp"

class Server
{
public:
    Server(const std::string& ip, uint16_t port);
    bool init(const std::string& ip, uint16_t port);
    void accept();
private:
    std::list<std::unique_ptr<ISocket>> m_clients;
};
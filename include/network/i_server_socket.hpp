#pragma once

#include <string>

#include "network/i_socket.hpp"

class IClientSocket;

class IServerSocket : public ISocket {
public:
    virtual void listen() = 0;
    virtual std::unique_ptr<IClientSocket> accept() = 0;
    virtual void bind(const std::string& ip, uint16_t port) = 0;
};
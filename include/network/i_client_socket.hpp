#pragma once

#include <string>

#include "network/i_socket.hpp"

class IClientSocket : public ISocket {
public:
    virtual void send(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> recieve(std::size_t size) = 0;
    virtual void connect(const std::string& ip, uint16_t port) = 0;
};
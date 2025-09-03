#pragma once

#include <memory>

#include "network/i_client_socket.hpp"

class IEventLoop
{
public:
    virtual void run() = 0;
    virtual void add_socket(std::unique_ptr<IClientSocket> socket) = 0;
    virtual void remove_socket(std::unique_ptr<IClientSocket> socket) = 0;
};
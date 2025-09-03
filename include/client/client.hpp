#pragma once

#include <memory>

#include "network/i_socket.hpp"

class Client
{
public:
private:
    std::unique_ptr<ISocket> server;
};
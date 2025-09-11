#pragma once

#include <memory>

#include "common/network/i_socket.hpp"

class Client
{
public:
private:
    std::unique_ptr<ISocket> server;
};
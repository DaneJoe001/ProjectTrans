#pragma once

#include "common/network/i_socket.hpp"

class ISocketMonitor
{
public:
    virtual bool is_readable(const ISocket* socket) = 0;
    virtual bool is_writeable(const ISocket* socket) = 0;
};
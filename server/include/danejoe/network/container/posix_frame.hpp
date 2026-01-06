#pragma once

#include "danejoe/network/container/buffer.hpp"

namespace DaneJoe
{
    struct PosixFrame
    {
        uint64_t connect_id;
        Buffer data;
    };
};
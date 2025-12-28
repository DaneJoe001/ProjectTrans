#pragma once

#include <cstdint>

#include "common/protocol/network_endpoint.hpp"

struct TransContext
{
    uint64_t request_id = 0;
    NetworkEndpoint endpoint;
};
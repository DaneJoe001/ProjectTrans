#pragma once

#include <string>
#include <cstdint>

struct NetworkEndpoint
{
    std::string ip;
    uint16_t port;
    bool operator==(const NetworkEndpoint& other) const
    {
        return ip == other.ip && port == other.port;
    }
};

namespace std
{
    template<>
    struct hash<NetworkEndpoint>
    {
        std::size_t operator()(const NetworkEndpoint& endpoint) const noexcept
        {
            std::size_t h1 = std::hash<std::string>{}(endpoint.ip);
            std::size_t h2 = std::hash<uint16_t>{}(endpoint.port);
            return h1 ^ (h2 << 1);
        }
    };
}
/**
 * @file network_endpoint.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 网络端点
 * @date 2025-12-28
 */
#pragma once

#include <string>
#include <cstdint>

/**
 * @brief 网络端点
 * @details 用于标识网络连接目标（ip:port）。
 *          支持相等比较，并提供 std::hash 特化以便作为 unordered_map/unordered_set 的键。
 */
struct NetworkEndpoint
{
    /// @brief IP地址
    std::string ip;
    /// @brief 端口号
    uint16_t port;
    /**
     * @brief 判断两个网络端点是否相等
     * @param other 另一个网络端点
     * @return 是否相等
     */
    bool operator==(const NetworkEndpoint& other) const
    {
        return ip == other.ip && port == other.port;
    }
};

/**
 * @namespace std
 * @brief std命名空间
 * @details 此处提供 std::hash<NetworkEndpoint> 的模板特化，便于 NetworkEndpoint 作为哈希容器键。
 */
namespace std
{
    /**
     * @brief 网络端点哈希
     * @details 通过组合 ip 与 port 的哈希值生成结果，用于容器键值散列。
     */
    template<>
    struct hash<NetworkEndpoint>
    {
        /**
         * @brief 哈希函数
         * @param endpoint 网络端点
         * @return 哈希值
         */
        std::size_t operator()(const NetworkEndpoint& endpoint) const noexcept
        {
            std::size_t h1 = std::hash<std::string>{}(endpoint.ip);
            std::size_t h2 = std::hash<uint16_t>{}(endpoint.port);
            return h1 ^ (h2 << 1);
        }
    };
}
/**
 * @file trans_context.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 传输上下文
 * @date 2025-12-28
 */
#pragma once

#include <cstdint>

#include "common/protocol/network_endpoint.hpp"

/**
 * @brief 传输上下文
 */
struct TransContext
{
    /// @brief 请求ID
    uint64_t request_id = 0;
    /// @brief 网络端点
    NetworkEndpoint endpoint;
};
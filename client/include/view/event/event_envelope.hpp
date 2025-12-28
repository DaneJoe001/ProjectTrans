#pragma once

#include <qnamespace.h>
#include <string>
#include <cstdint>
#include <chrono>

struct EventContext
{
    std::string m_object_name;
};

/**
 * @str
 *  @brief 事件封包
 */
struct EventEnvelope
{
    uint64_t m_event_id = 0;
    EventContext m_event_context;
    std::chrono::steady_clock::time_point m_event_time;
};
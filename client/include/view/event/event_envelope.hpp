/**
 * @file event_envelope.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 事件封包相关结构定义
 * @date 2025-12-28
 */
#pragma once

#include <qnamespace.h>
#include <string>
#include <cstdint>
#include <chrono>

/**
 * @struct EventContext
 * @brief 事件上下文
 * @details 包含事件的来源信息，用于标识事件的发起者
 */
struct EventContext
{
    /// @brief 对象名称，标识发起事件的对象
    std::string m_object_name;
};

/**
 * @struct EventEnvelope
 * @brief 事件封包
 * @details 封装事件的完整信息，包括事件ID、上下文和时间戳，用于事件追踪和匹配
 */
struct EventEnvelope
{
    /// @brief 事件ID，唯一标识一个事件
    uint64_t m_event_id = 0;
    /// @brief 事件上下文，包含事件的来源信息
    EventContext m_event_context;
    /// @brief 事件时间戳，记录事件发生的时间点
    std::chrono::steady_clock::time_point m_event_time;
};
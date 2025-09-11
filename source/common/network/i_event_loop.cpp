#include "common/network/i_event_loop.hpp"

bool IEventLoop::is_event_valid(EventType type)
{
    return type != EventType::None;
}

IEventLoop::EventType operator|(IEventLoop::EventType lhs, IEventLoop::EventType rhs)
{
    return static_cast<IEventLoop::EventType>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

IEventLoop::EventType operator&(IEventLoop::EventType lhs, IEventLoop::EventType rhs)
{
    return static_cast<IEventLoop::EventType>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
#include "common/network/i_event_loop.hpp"

std::atomic<bool> IEventLoop::m_is_running = false;

bool IEventLoop::is_event_valid(EventType type)
{
    // 当事件类型不为空即为有效
    return type != EventType::None;
}

IEventLoop::EventType operator|(IEventLoop::EventType lhs, IEventLoop::EventType rhs)
{
    // 按位或
    return static_cast<IEventLoop::EventType>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

IEventLoop::EventType operator&(IEventLoop::EventType lhs, IEventLoop::EventType rhs)
{
    // 按位与
    return static_cast<IEventLoop::EventType>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

void IEventLoop::end_loop()
{
    // 更新运行状态
    m_is_running.store(false);
}
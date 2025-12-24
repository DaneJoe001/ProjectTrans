#include "danejoe/network/event_loop/i_event_loop.hpp"

#include "danejoe/common/enum/enum_flag.hpp"

bool DaneJoe::IEventLoop::is_event_valid(EventLoopEventType type)
{
    // 当事件类型不为空即为有效
    return type != EventLoopEventType::None;
}

std::string DaneJoe::to_string(EventLoopEventType type)
{
    if (type == EventLoopEventType::None)
    {
        return ENUM_TO_STRING(EventLoopEventType::None);
    }
    if (type == EventLoopEventType::Readable)
    {
        return ENUM_TO_STRING(EventLoopEventType::Readable);
    }
    if (type == EventLoopEventType::Writable)
    {
        return ENUM_TO_STRING(EventLoopEventType::Writable);
    }
    if (type == EventLoopEventType::Priority)
    {
        return ENUM_TO_STRING(EventLoopEventType::Priority);
    }
    if (type == EventLoopEventType::Disconnected)
    {
        return ENUM_TO_STRING(EventLoopEventType::Disconnected);
    }
    if (type == EventLoopEventType::EdgeTriggered)
    {
        return ENUM_TO_STRING(EventLoopEventType::EdgeTriggered);
    }
    if (type == EventLoopEventType::OneShot)
    {
        return ENUM_TO_STRING(EventLoopEventType::OneShot);
    }
    if (type == EventLoopEventType::Error)
    {
        return ENUM_TO_STRING(EventLoopEventType::Error);
    }
    if (type == EventLoopEventType::PeerClosed)
    {
        return ENUM_TO_STRING(EventLoopEventType::PeerClosed);
    }

    std::string result;
    if (has_flag(type, EventLoopEventType::Readable))
    {
        result += ENUM_TO_STRING(EventLoopEventType::Readable);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::Writable))
    {
        result += ENUM_TO_STRING(EventLoopEventType::Writable);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::Priority))
    {
        result += ENUM_TO_STRING(EventLoopEventType::Priority);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::Disconnected))
    {
        result += ENUM_TO_STRING(EventLoopEventType::Disconnected);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::EdgeTriggered))
    {
        result += ENUM_TO_STRING(EventLoopEventType::EdgeTriggered);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::OneShot))
    {
        result += ENUM_TO_STRING(EventLoopEventType::OneShot);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::Error))
    {
        result += ENUM_TO_STRING(EventLoopEventType::Error);
        result += "|";
    }
    if (has_flag(type, EventLoopEventType::PeerClosed))
    {
        result += ENUM_TO_STRING(EventLoopEventType::PeerClosed);
        result += "|";
    }
    if (!result.empty())
    {
        result.pop_back();
        return result;
    }
    return ENUM_TO_STRING(EventLoopEventType::None);
}

template<>
DaneJoe::EventLoopEventType DaneJoe::enum_cast<DaneJoe::EventLoopEventType>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(EventLoopEventType::None))
    {
        return EventLoopEventType::None;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::Readable))
    {
        return EventLoopEventType::Readable;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::Writable))
    {
        return EventLoopEventType::Writable;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::Priority))
    {
        return EventLoopEventType::Priority;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::Disconnected))
    {
        return EventLoopEventType::Disconnected;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::EdgeTriggered))
    {
        return EventLoopEventType::EdgeTriggered;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::OneShot))
    {
        return EventLoopEventType::OneShot;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::Error))
    {
        return EventLoopEventType::Error;
    }
    else if (enum_string == ENUM_TO_STRING(EventLoopEventType::PeerClosed))
    {
        return EventLoopEventType::PeerClosed;
    }
    return EventLoopEventType::None;
}

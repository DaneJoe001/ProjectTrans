#include "danejoe/condition/single_interval.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

std::string DaneJoe::to_string(IntervalState state)
{
    switch (state)
    {
    case IntervalState::Empty:
        return ENUM_TO_STRING(IntervalState::Empty);
    case IntervalState::Whole:
        return ENUM_TO_STRING(IntervalState::Whole);
    case IntervalState::FixedLeft:
        return ENUM_TO_STRING(IntervalState::FixedLeft);
    case IntervalState::FixedRight:
        return ENUM_TO_STRING(IntervalState::FixedRight);
    case IntervalState::FixedBoth:
        return ENUM_TO_STRING(IntervalState::FixedBoth);
    case IntervalState::Unknown:
    default:
        return ENUM_TO_STRING(IntervalState::Unknown);
    }
}

template<>
DaneJoe::IntervalState DaneJoe::enum_cast<DaneJoe::IntervalState>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(IntervalState::Empty))
    {
        return IntervalState::Empty;
    }
    else if (enum_string == ENUM_TO_STRING(IntervalState::Whole))
    {
        return IntervalState::Whole;
    }
    else if (enum_string == ENUM_TO_STRING(IntervalState::FixedLeft))
    {
        return IntervalState::FixedLeft;
    }
    else if (enum_string == ENUM_TO_STRING(IntervalState::FixedRight))
    {
        return IntervalState::FixedRight;
    }
    else if (enum_string == ENUM_TO_STRING(IntervalState::FixedBoth))
    {
        return IntervalState::FixedBoth;
    }
    return IntervalState::Unknown;
}

template<>
DaneJoe::BoundarySide DaneJoe::enum_cast<DaneJoe::BoundarySide>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(BoundarySide::Left))
    {
        return BoundarySide::Left;
    }
    else if (enum_string == ENUM_TO_STRING(BoundarySide::Right))
    {
        return BoundarySide::Right;
    }
    // BoundarySide 没有 Unknown，默认回退到 Left
    return BoundarySide::Left;
}
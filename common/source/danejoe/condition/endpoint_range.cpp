#include "danejoe/condition/endpoint_range.hpp"

std::string DaneJoe::to_string(BoundarySide side)
{
    switch (side)
    {
    case BoundarySide::Left:
        return ENUM_TO_STRING(BoundarySide::Left);
    case BoundarySide::Right:
        return ENUM_TO_STRING(BoundarySide::Right);
    default:
        return ENUM_TO_STRING(BoundarySide::Unknown);
    }
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
    else if (enum_string == ENUM_TO_STRING(BoundarySide::Unknown))
    {
        return BoundarySide::Unknown;
    }
    return BoundarySide::Unknown;
}
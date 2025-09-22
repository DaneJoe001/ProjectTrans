#pragma once

#include <cstdint>
#include <string>

class ScreenUtil
{
public:
    struct PointInfo
    {
        int32_t x = -1;
        int32_t y = -1;
        PointInfo();
        PointInfo(int x, int y);
        PointInfo(QPoint point);
        std::string to_string() const;
        QPoint to_qpoint() const;
    };
    struct RectInfo
    {
        PointInfo pos;
        PointInfo size;
        RectInfo();
        RectInfo(PointInfo pos, PointInfo size);
        RectInfo(int x, int y, int width, int height);
        RectInfo(QRect rect);
        std::string to_string() const;
        QRect to_qrect() const;
    };
    enum class RealativePosition
    {
        Center,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };
public:
    static RectInfo get_screen_rect(uint32_t screen_index = 0);
    static PointInfo get_destination_point(const RectInfo& parent_window_rect, const RectInfo& window_rect, RealativePosition position);
};
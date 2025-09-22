#include <format>

#include <QList>
#include <QApplication>
#include <QScreen>

#include "common/util/screen_util.hpp"
#include "log/manage_logger.hpp"

ScreenUtil::PointInfo::PointInfo() {}
ScreenUtil::PointInfo::PointInfo(int x, int y)
{
    this->x = x;
    this->y = y;
}
ScreenUtil::PointInfo::PointInfo(QPoint point)
{
    this->x = point.x();
    this->y = point.y();
}
QPoint ScreenUtil::PointInfo::to_qpoint() const
{
    return QPoint(x, y);
}

std::string ScreenUtil::PointInfo::to_string() const
{
    return std::format("Point: ({}, {})", x, y);
}

std::string ScreenUtil::RectInfo::to_string() const
{
    return std::format("Rect: ({}, {}, {}, {})", pos.x, pos.y, size.x, size.y);
}

ScreenUtil::RectInfo::RectInfo() {}
ScreenUtil::RectInfo::RectInfo(PointInfo pos, PointInfo size)
{
    this->pos = pos;
    this->size = size;
}
ScreenUtil::RectInfo::RectInfo(int x, int y, int width, int height)
{
    this->pos = PointInfo(x, y);
    this->size = PointInfo(width, height);
}
ScreenUtil::RectInfo::RectInfo(QRect rect)
{
    this->pos = PointInfo(rect.x(), rect.y());
    this->size = PointInfo(rect.width(), rect.height());
}
QRect ScreenUtil::RectInfo::to_qrect() const
{
    DANEJOE_LOG_WARN("default", "ScreenUtil", "Incomplete match QRect");
    return QRect(pos.to_qpoint(), size.to_qpoint());
}

ScreenUtil::RectInfo ScreenUtil::get_screen_rect(uint32_t screen_index)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    DANEJOE_LOG_TRACE("default", "ScreenUtil", "Screen count: {}", screens.size());
    if (screen_index >= screens.size())
    {
        DANEJOE_LOG_WARN("default", "ScreenUtil", "Screen index out of range, using default screen.");
        screen_index = 0;
    }
    QScreen* screen = screens[screen_index];
    return RectInfo(screen->geometry());
}
ScreenUtil::PointInfo ScreenUtil::get_destination_point(const RectInfo& parent_window_rect, const RectInfo& window_rect, RealativePosition position)
{
    int dest_x = -1;
    int dest_y = -1;
    switch (position)
    {
    case RealativePosition::Center:
    {
        dest_x = (parent_window_rect.size.x - window_rect.size.x) / 2;
        dest_y = (parent_window_rect.size.y - window_rect.size.y) / 2;
        break;
    }
    default:
    {
        dest_x = (parent_window_rect.size.x - window_rect.size.x) / 2;
        dest_y = (parent_window_rect.size.y - window_rect.size.y) / 2;
        break;
    }
    }
    return PointInfo(dest_x, dest_y);
}
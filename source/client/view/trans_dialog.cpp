#include "client/view/trans_dialog.hpp"
#include <danejoe/logger/logger_manager.hpp>
#include "common/util/screen_util.hpp"

TransDialog::TransDialog(QWidget* parent)
    : QDialog(parent)
{

}

void TransDialog::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "TransDialog", "TransDialog has been initialized");
    }
    setWindowTitle("Trans Dialog");
    ScreenUtil::RectInfo screen_rect = { 450, 250, 400, 500 };
    auto parent_window = this->parentWidget();
    if (parent_window)
    {
        auto relative_point = ScreenUtil::get_destination_point(parent_window->geometry(), screen_rect, ScreenUtil::RealativePosition::Center);
        QPoint parent_pos = parent_window->pos();
        screen_rect.pos.x = relative_point.x + parent_pos.x();
        screen_rect.pos.y = relative_point.y + parent_pos.y();
    }
    this->setGeometry(screen_rect.pos.x, screen_rect.pos.y, screen_rect.size.x, screen_rect.size.y);
    DANEJOE_LOG_TRACE("default", "TransDialog", "Window rect: {}", screen_rect.to_string());
    m_is_init = true;
}
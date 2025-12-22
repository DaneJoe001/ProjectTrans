#include <danejoe/logger/logger_manager.hpp>

#include "server/view/connection_info_widget.hpp"

ConnectionInfoWidget::ConnectionInfoWidget(QWidget* parent)
    : QWidget(parent)
{}
void ConnectionInfoWidget::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ConnectionInfoWidget", "ConnectionInfoWidget has already been initialized");
        return;
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setObjectName("connection_info_widget");
    m_is_init = true;
}
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>

#include "server/view/resource_info_widget.hpp"
#include "server/model/server_file_info_table_model.hpp"
#include "log/manage_logger.hpp"

ResourceInfoWidget::ResourceInfoWidget(QWidget* parent) : QWidget(parent) {}
void ResourceInfoWidget::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "ResourceInfoWidget", "ResourceInfoWidget has been initialized");
        return;
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setObjectName("resource_info_widget");
    m_model = ServerFileInfoTableModel::get_instance();
    m_model->init();
    m_table_view = new QTableView(this);
    m_table_view->setModel(m_model);
    m_table_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_layout = new QVBoxLayout(this);

    m_layout->addWidget(m_table_view);
    m_layout->setObjectName("resource_info_widget_layout");
    m_layout->setStretch(0, 1);
    m_is_init = true;
}
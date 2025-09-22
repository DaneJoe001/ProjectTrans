#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include "client/view/file_trans_info_widget.hpp"
#include "client/model/file_trans_info_table_model.hpp"
#include "log/manage_logger.hpp"

FileTransInfoWidget::FileTransInfoWidget(QWidget* parent) : QWidget(parent)
{

}

void FileTransInfoWidget::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "FileTransInfoWidget", "FileTransInfoWidget has been initialized");
        return;
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout = new QVBoxLayout(this);
    m_table_view = new QTableView(this);
    m_model = FileTransInfoTableModel::get_instance();
    m_model->init();
    m_table_view->setModel(m_model);
    m_table_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_layout->addWidget(m_table_view);
    m_layout->setStretch(0, 1);
    m_is_init = true;
}
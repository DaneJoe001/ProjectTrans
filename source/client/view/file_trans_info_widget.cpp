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

    connect(m_table_view, &QTableView::clicked, this, &FileTransInfoWidget::on_cell_clicked);

    m_is_init = true;
}

void FileTransInfoWidget::update_view(int32_t file_id, int32_t block_id)
{
    DANEJOE_LOG_TRACE("default", "FileTransInfoWidget", "update_view: file_id {}, block_id {}", file_id, block_id);
    m_model->update(file_id, block_id);
}

void FileTransInfoWidget::on_cell_clicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "FileTransInfoWidget", "on_cell_clicked: row {}, column {}", index.row(), index.column());
    emit row_clicked(index.row());
}
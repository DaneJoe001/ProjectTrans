#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include "danejoe/logger/logger_manager.hpp"
#include "view/widget/task_table_widget.hpp"
#include "model/view/task_table_model.hpp"


TaskTableWidget::TaskTableWidget(QWidget* parent) : QWidget(parent) {}

void TaskTableWidget::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "TaskTableWidget", "TaskTableWidget has been initialized");
        return;
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout = new QVBoxLayout(this);
    m_table_view = new QTableView(this);
    m_model = TaskTableModel::get_instance();
    m_model->init();
    m_table_view->setModel(m_model);
    m_table_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_layout->addWidget(m_table_view);
    m_layout->setStretch(0, 1);

    m_is_init = true;
}

void TaskTableWidget::update_view(int32_t file_id, int32_t block_id)
{
    DANEJOE_LOG_TRACE("default", "TaskTableWidget", "update_view: file_id {}, block_id {}", file_id, block_id);
}

void TaskTableWidget::on_cell_clicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "TaskTableWidget", "on_cell_clicked: row {}, column {}", index.row(), index.column());
    emit row_clicked(index.row());
}
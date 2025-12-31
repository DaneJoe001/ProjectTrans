#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include "danejoe/logger/logger_manager.hpp"
#include "view/widget/task_table_widget.hpp"
#include "model/view/task_table_model.hpp"


TaskTableWidget::TaskTableWidget(
    TaskService& task_service,
    QPointer<ViewEventHub> view_event_hub,
    QPointer<TaskTableModel>table_model,
    QWidget* parent) :
    QWidget(parent),
    m_task_service(task_service),
    m_view_event_hub(view_event_hub),
    m_table_model(table_model)
{}

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
    m_table_model->init();
    m_table_view->setModel(m_table_model);
    m_table_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_layout->addWidget(m_table_view);
    m_layout->setStretch(0, 1);

    connect(m_view_event_hub, &ViewEventHub::task_entity_add, this, &TaskTableWidget::on_task_entity_add);
    connect(m_table_view, &QTableView::clicked, this, &TaskTableWidget::on_cell_clicked);

    m_is_init = true;
}

void TaskTableWidget::on_cell_clicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }
    DANEJOE_LOG_TRACE("default", "TaskTableWidget", "on_cell_clicked: row {}, column {}", index.row(), index.column());
    auto task_opt = m_table_model->get_task_by_row(index.row());
    if (task_opt.has_value())
    {
        emit task_selected(task_opt->task_id);
    }
}

void TaskTableWidget::on_task_entity_add(TaskEntity task_entity)
{
    DANEJOE_LOG_TRACE("default", "TaskTableWidget", "On task entity add!");
    m_table_model->add(task_entity);
}
/**
 * @file task_table_widget.hpp
 * @brief 任务列表窗口
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <QWidget>
#include <QPointer>

#include "model/view/task_table_model.hpp"
#include "view/event/view_event_hub.hpp"
#include "service/task_service.hpp"

class QVBoxLayout;
class QTableView;
class TaskTableModel;

/**
 * @class TaskTableWidget
 * @brief 任务列表窗口
 * @details 展示任务列表（QTableView + TaskTableModel），并在用户选择行时发出信号。
 */
class TaskTableWidget :public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param task_service 任务服务
     * @param view_event_hub 视图事件中心
     * @param table_model 任务列表模型
     * @param parent 父窗口
     */
    TaskTableWidget(
        TaskService& task_service,
        QPointer<ViewEventHub> view_event_hub,
        QPointer<TaskTableModel>table_model,
        QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
signals:
    /**
     * @brief 行点击信号
     * @param task_id 任务ID
     */
    void task_selected(int64_t task_id);
public slots:
    /**
     * @brief 单元格点击槽函数
     */
    void on_cell_clicked(const QModelIndex& index);
    /**
     * @brief 更新视图槽函数
     * @param task_entity 任务实体
     */
    void on_task_entity_add(TaskEntity task_entity);
    void on_task_update(int64_t task_id);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    /// @brief 任务服务
    TaskService& m_task_service;
    /// @brief 视图事件中心
    QPointer<ViewEventHub> m_view_event_hub = nullptr;
    /// @brief 布局
    QVBoxLayout* m_layout;
    /// @brief 表格视图
    QTableView* m_table_view;
    /// @brief 模型
    QPointer<TaskTableModel> m_table_model;
};
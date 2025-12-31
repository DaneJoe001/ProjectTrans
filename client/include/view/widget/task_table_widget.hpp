#pragma once

/**
 * @file file_trans_info_widget.hpp
 * @brief 文件传输信息窗口
 * @author DaneJoe001
 */

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
 * @brief 文件传输信息窗口
 */
class TaskTableWidget :public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
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
     * @param task_id 行
     */
    void task_selected(int64_t task_id);
public slots:
    /**
     * @brief 单元格点击槽函数
     */
    void on_cell_clicked(const QModelIndex& index);
    /**
     * @brief 更新视图槽函数
     * @param file_id 文件ID
     * @param block_id 块ID
     */
    void on_task_entity_add(TaskEntity task_entity);
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
    TaskService& m_task_service;
    QPointer<ViewEventHub> m_view_event_hub = nullptr;
    /// @brief 布局
    QVBoxLayout* m_layout;
    /// @brief 表格视图
    QTableView* m_table_view;
    /// @brief 模型
    QPointer<TaskTableModel> m_table_model;
};
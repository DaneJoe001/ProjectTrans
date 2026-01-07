/**
 * @file task_table_model.hpp
 * @brief 任务列表 TableModel
 * @author DaneJoe001
 * @date 2026-01-06
 */
#pragma once

#include <cstdint>

#include <QAbstractTableModel>
#include <QList>

#include "service/task_service.hpp"

 /**
  * @class TaskTableModel
  * @brief 任务列表 TableModel
  * @details 基于 QAbstractTableModel，为视图层提供任务列表的数据展示能力。
  */
class TaskTableModel : public QAbstractTableModel
{
public:
    /**
     * @brief 构造函数
     * @param task_view_service 任务服务
     * @param parent 父对象
     */
    TaskTableModel(
        TaskService& task_view_service,
        QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~TaskTableModel();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 更新
     * @param task_id 任务ID
     */
    void update(int64_t task_id);
    /**
     * @brief 列数
     * @param parent 父索引
     * @return 列数
     */
    int32_t columnCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 行数
     * @param parent 父索引
     * @return 行数
     */
    int32_t rowCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 数据
     * @param index 索引
     * @param role 角色
     * @return 数据
     */
    QVariant data(
        const QModelIndex& index,
        int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 头部数据
     * @param section 节
     * @param orientation 方向
     * @param role 角色
     * @return 头部数据
     */
    QVariant headerData(
        int32_t section,
        Qt::Orientation orientation,
        int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 添加
     * @param task_info 任务信息
     */
    void add(const TaskEntity& task_info);
    /**
     * @brief 删除
     * @param task_info 任务信息
     */
    void remove(const TaskEntity& task_info);
    /**
     * @brief 通过行号获取任务
     * @param row 行号
     * @return 对应任务（不存在时返回空）
     */
    std::optional<TaskEntity> get_task_by_row(int64_t row);
private:
    /// @brief 任务服务引用
    TaskService& m_task_service;
    /// @brief 任务列表
    QList<TaskEntity> m_task_list;
};
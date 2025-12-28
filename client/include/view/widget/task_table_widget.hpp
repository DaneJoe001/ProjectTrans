#pragma once

/**
 * @file file_trans_info_widget.hpp
 * @brief 文件传输信息窗口
 * @author DaneJoe001
 */

#include <QWidget>

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
    TaskTableWidget(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
signals:
    /**
     * @brief 行点击信号
     * @param row 行
     */
    void row_clicked(int32_t row);
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
    void update_view(int32_t file_id, int32_t block_id);
private:
    /// @brief 布局
    QVBoxLayout* m_layout;
    /// @brief 表格视图
    QTableView* m_table_view;
    /// @brief 模型
    TaskTableModel* m_model;
    /// @brief 是否已初始化
    bool m_is_init = false;
};
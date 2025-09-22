#pragma once

/**
 * @file file_trans_info_widget.hpp
 * @brief 文件传输信息窗口
 * @author DaneJoe001
 */

#include <QWidget>

class QVBoxLayout;
class QTableView;
class FileTransInfoTableModel;

/**
 * @class FileTransInfoWidget
 * @brief 文件传输信息窗口
 */
class FileTransInfoWidget :public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    FileTransInfoWidget(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 布局
    QVBoxLayout* m_layout;
    /// @brief 表格视图
    QTableView* m_table_view;
    /// @brief 模型
    FileTransInfoTableModel* m_model;
    /// @brief 是否已初始化
    bool m_is_init = false;
};
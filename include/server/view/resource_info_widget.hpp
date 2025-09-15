#pragma once

/**
 * @file resource_info_widget.hpp
 * @brief 资源信息小部件
 * @author DaneJoe001
 */



#include <QWidget>

class ServerFileInfoTableModel;
class QTableView;
class QVBoxLayout;

/**
 * @class ResourceInfoWidget
 * @brief 资源信息小部件
 */
class ResourceInfoWidget : public QWidget {
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ResourceInfoWidget(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 模型
    ServerFileInfoTableModel* m_model = nullptr;
    /// @brief 表格视图
    QTableView* m_table_view = nullptr;
    /// @brief 布局
    QVBoxLayout* m_layout = nullptr;
};
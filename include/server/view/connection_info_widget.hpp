#pragma once

/**
 * @file connection_info_widget.hpp
 * @brief 连接信息小部件
 * @author DaneJoe001
 */


#include <QWidget>

 /**
  * @class ConnectionInfoWidget
  * @brief 连接信息小部件
  */
class ConnectionInfoWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ConnectionInfoWidget(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
};
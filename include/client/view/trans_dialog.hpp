#pragma once

/**
 * @file trans_dialog.hpp
 * @brief 传输对话框
 * @author DaneJoe001
 */

#include <QDialog>

 /**
  * @class TransDialog
  * @brief 传输对话框
  */
class TransDialog :public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    TransDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
    /// @brief 是否已初始化
    bool m_is_init = false;
};
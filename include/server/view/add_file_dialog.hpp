#pragma once

/**
 * @file add_file_dialog.hpp
 * @brief 添加文件对话框
 * @author DaneJoe001
 */


#include <QDialog>

#include "server/service/server_file_info_service.hpp"

class QLabel;
class QLineEdit;
class QPushButton;
class QFileDialog;
class QHBoxLayout;

/**
 * @class AddFileDialog
 * @brief 添加文件对话框
 */
class AddFileDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    AddFileDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
public slots:
    /**
     * @brief 文件路径按钮点击
     */
    void on_file_path_button_clicked();
    /**
     * @brief 添加文件按钮点击
     */
    void on_add_file_button_clicked();
private:
    /// @brief 文件路径标签
    QLabel* m_file_path_label;
    /// @brief 文件路径编辑框
    QLineEdit* m_file_path_line_edit;
    /// @brief 文件路径按钮
    QPushButton* m_file_path_button;
    /// @brief 添加文件按钮
    QPushButton* m_add_file_button;
    /// @brief 文件对话框
    QFileDialog* m_file_dialog;
    /// @brief 文件路径布局
    QHBoxLayout* m_file_path_layout;
    /// @brief 文件信息服务
    ServerFileInfoService m_file_info_service;
};
#pragma once

/**
 * @file file_info_dialog.hpp
 * @brief 文件信息对话框
 * @author DaneJoe001
 */

#include <QDialog>

/**
 * @class FileInfoDialog
 * @brief 文件信息对话框
 */
class FileInfoDialog : public QDialog 
{
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    FileInfoDialog(QWidget* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
private:
};
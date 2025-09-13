#pragma once

/**
 * @file file_info_dialog.hpp
 * @brief 文件信息对话框
 */

#include <QDialog>

class FileInfoDialog : public QDialog {
public:
    Q_OBJECT
public:
    FileInfoDialog(QWidget* parent = nullptr);
    void init();
private:
};
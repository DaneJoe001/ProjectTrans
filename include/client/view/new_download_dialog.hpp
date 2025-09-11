#pragma once

#include <QDialog>

class NewDownloadDialog : public QDialog
{
    Q_OBJECT
public:
    NewDownloadDialog(QWidget* parent = nullptr);
    void init();
};
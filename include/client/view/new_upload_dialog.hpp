#pragma once

#include <QDialog>

class NewUploadDialog : public QDialog
{
    Q_OBJECT
public:
    NewUploadDialog(QWidget* parent = nullptr);
    void init();
};
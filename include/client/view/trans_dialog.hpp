#pragma once

#include <QDialog>

class TransDialog :public QDialog
{
    Q_OBJECT
public:
    TransDialog(QWidget* parent = nullptr);
    void init();
};
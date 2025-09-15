#pragma once

#include <QWidget>

class QVBoxLayout;
class QTableView;
class FileTransInfoTableModel;

class FileTransInfoWidget :public QWidget
{
    Q_OBJECT
public:
    FileTransInfoWidget(QWidget* parent = nullptr);
    void init();
private:
    QVBoxLayout* m_layout;
    QTableView* m_table_view;
    FileTransInfoTableModel* m_model;
};
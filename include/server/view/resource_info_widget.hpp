#pragma once

#include <QWidget>

class ServerFileInfoTableModel;
class QTableView;
class QVBoxLayout;

class ResourceInfoWidget : public QWidget {
public:
    Q_OBJECT
public:
    explicit ResourceInfoWidget(QWidget* parent = nullptr);
    void init();
private:
    ServerFileInfoTableModel* m_model = nullptr;
    QTableView* m_table_view = nullptr;
    QVBoxLayout* m_layout = nullptr;
};
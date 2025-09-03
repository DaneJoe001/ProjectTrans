#pragma once

#include <QMainWindow>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QString m_window_title = "Client";
};
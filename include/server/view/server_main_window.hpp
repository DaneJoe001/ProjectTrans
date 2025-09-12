#pragma once

#include <QMainWindow>

class QStackedWidget;
class QAction;

class ServerMainWindow : public QMainWindow {
public:
    Q_OBJECT
public:
    ServerMainWindow(QWidget* parent = nullptr);
    void init();
    void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
private:
    QStackedWidget* m_stacked_widget = nullptr;
};
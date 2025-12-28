#pragma once

#include <QObject>
#include <QEvent>

class QKeyEvent;

class ServerThread;
class ServerMainWindow;

class ServerApp : public QObject
{
    Q_OBJECT
public:
    ServerApp(QObject* parent = nullptr);
    ~ServerApp();
    void init();
    bool eventFilter(QObject* watched, QEvent* event) override;
    void show_main_window();
    void hide_main_window();
private:
    void init_logger();
    void clear_log();
    void init_database();
    void clear_database();

private:
    bool m_is_init = false;
    bool m_is_main_window_visible = false;
    ServerThread* m_server_thread = nullptr;
    ServerMainWindow* m_server_main_window = nullptr;
};
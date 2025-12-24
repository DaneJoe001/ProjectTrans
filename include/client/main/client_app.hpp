#pragma once

#include <QObject>

class ClientMainWindow;
class TransManager;

class ClientApp : public QObject
{
    Q_OBJECT
public:
    ClientApp(QObject* parent = nullptr);
    ~ClientApp();
    void init();
    void show_main_window();
private:
    void init_database();
    void clear_database();
    void init_logger();
    void clear_log();

private:
    bool m_is_init = false;
    ClientMainWindow* m_main_window = nullptr;
    /// @brief 传输管理器
    TransManager* m_trans_manager = nullptr;
};
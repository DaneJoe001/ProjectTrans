#pragma once

#include <memory>
#include <thread>

#include <QObject>
#include <QEvent>

#include "danejoe/network/runtime/reactor_mail_box.hpp"

#include "runtime/network_runtime.hpp"
#include "runtime/business_runtime.hpp"

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
    void stop();
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

    std::thread m_business_thread;
    std::thread m_network_thread;
    std::shared_ptr<NetworkRuntime> m_network_runtime = nullptr;
    std::shared_ptr<BusinessRuntime> m_bussiness_runtime = nullptr;
    std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;

    ServerMainWindow* m_server_main_window = nullptr;
};
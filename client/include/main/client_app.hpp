#pragma once

#include <QObject>
#include <QPointer>

#include "service/trans_service.hpp"
#include "service/block_service.hpp"
#include "service/client_file_service.hpp"

#include "view/event/view_event_hub.hpp"

class ClientMainWindow;
class ViewEventController;

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
    QPointer<ViewEventHub> m_view_event_hub;
    ClientMainWindow* m_main_window = nullptr;
    ViewEventController* m_view_event_controller = nullptr;
    TransService& m_trans_service;
    BlockService& m_block_service;
    ClientFileService& m_client_file_service;
};
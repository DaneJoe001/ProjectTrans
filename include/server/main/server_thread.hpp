#pragma once

#include <QThread>

#include "danejoe/network/event_loop/epoll_event_loop.hpp"

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QObject* parent = nullptr);
    ~ServerThread();
    void init();
    void run() override;
    void stop();

private:
    DaneJoe::EpollEventLoop m_event_loop;
};
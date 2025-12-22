#pragma once

#include <QThread>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QObject* parent = nullptr);
    ~ServerThread();

    void run() override;
    void stop();
};
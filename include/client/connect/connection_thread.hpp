#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <QThread>
#include <QTimer>

#include "client/connect/client_connection.hpp"

class ConnectionThread : public QThread
{
    Q_OBJECT
public:
    ConnectionThread(QObject* parent = nullptr);
    bool init(std::unique_ptr<ClientConnection> connection);
    bool init(const std::string& ip, uint16_t port);
    void run() override;
    void stop();
    ~ConnectionThread();
signals:
    void data_recieved_signal(const std::vector<uint8_t>& data);
public slots:
    void data_send_slot(const std::vector<uint8_t>& data);
private:
    std::unique_ptr<ClientConnection> m_connection = nullptr;
    bool m_is_running = false;
    QTimer m_timer;
};
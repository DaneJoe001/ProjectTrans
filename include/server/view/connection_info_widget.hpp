#pragma once

#include <QWidget>

class ConnectionInfoWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConnectionInfoWidget(QWidget* parent = nullptr);
    void init();
};
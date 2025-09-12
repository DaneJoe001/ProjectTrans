#pragma once

#include <QWidget>

class ResourceInfoWidget : public QWidget {
public:
    Q_OBJECT
public:
    explicit ResourceInfoWidget(QWidget* parent = nullptr);
    void init();
};
#pragma once

#include <QObject>
#include <QDebug>

class PowerClient : public QObject
{
    Q_OBJECT
public:
    explicit PowerClient(QObject *parent = nullptr);
    void requestInitialBatteryLevel();
signals:
    void batteryLevelChanged(int level);
};
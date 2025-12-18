#include "PowerClient.h"

PowerClient::PowerClient(QObject *parent) : QObject(parent)
{
    qDebug() << "PowerClient: Initialized.";
}

void PowerClient::requestInitialBatteryLevel()
{
    qDebug() << "PowerClient: Requesting initial battery level from system service.";
    // In a real app, this makes a D-Bus call to get current state.
}
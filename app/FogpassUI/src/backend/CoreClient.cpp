#include "CoreClient.h"

CoreClient::CoreClient(QObject *parent) : QObject(parent)
{
    qDebug() << "CoreClient: Initialized.";
}

void CoreClient::setMode(const QString &mode)
{
    qDebug() << "CoreClient: Simulating D-Bus call to 'core' service to set mode to:" << mode;
    // In a real application, this is where you would make the D-Bus call
    // to the 'fogpass-core' system service.
    // For now, we'll just emit the signal immediately to simulate success.
    emit modeChanged(mode);
}
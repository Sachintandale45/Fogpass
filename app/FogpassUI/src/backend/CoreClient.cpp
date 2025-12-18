#include "CoreClient.h"
#include <QDBusConnection> // New: For QDBusConnection

CoreClient::CoreClient(QObject *parent) : QObject(parent)
{
    // D-Bus CLIENT setup (core service)
    m_iface = new QDBusInterface(
        "com.fogpass.Core",          // service name
        "/com/fogpass/Core",         // object path
        "com.fogpass.Core",          // interface
        QDBusConnection::systemBus(),
        this
    );
    qDebug() << "CoreClient: Initialized and connected to D-Bus service 'com.fogpass.Core'.";
}

void CoreClient::setWeatherMode(bool foggy) // Renamed and changed to bool
{
    // Fire-and-forget for now - no response handling needed
    m_iface->call("SetWeatherMode", foggy);
    qDebug() << "CoreClient: D-Bus call 'SetWeatherMode' sent with value:" << foggy;
    // No emit modeChanged(mode); here as per the "No response handling needed" instruction.
    // The UI will update when the system backend eventually sends a state update signal.
}

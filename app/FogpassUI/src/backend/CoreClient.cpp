#include "CoreClient.h"
#include <QDBusConnection> // New: For QDBusConnection
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

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

    // Connect the D-Bus signal from the service to our local C++ signal
    // This uses the older string-based syntax, which is required for D-Bus signals.
    bool connected_legacy = QObject::connect(m_iface, SIGNAL(LandmarkLocationsChanged(QStringList)),
                                      this, SIGNAL(landmarkLocationsChanged(QStringList)));
    qDebug() << "CoreClient: Connection to LandmarkLocationsChanged signal:" << (connected_legacy ? "successful" : "failed");

    bool connected_new = QObject::connect(m_iface, SIGNAL(NextLandmarksUpdated(QString,int,QString,int,QString,int)),
                                          this, SIGNAL(landmarksUpdated(QString,int,QString,int,QString,int)));
    qDebug() << "CoreClient: Connection to NextLandmarksUpdated signal:" << (connected_new ? "successful" : "failed");
}

void CoreClient::setWeatherMode(bool foggy) // Renamed and changed to bool
{
    // Fire-and-forget for now - no response handling needed
    m_iface->call("SetWeatherMode", foggy);
    qDebug() << "CoreClient: D-Bus call 'SetWeatherMode' sent with value:" << foggy;
    // No emit modeChanged(mode); here as per the "No response handling needed" instruction.
    // The UI will update when the system backend eventually sends a state update signal.
}

void CoreClient::setGnssMode(int mode)
{
    m_iface->call("SetGnssMode", mode);
    qDebug() << "CoreClient: D-Bus call 'SetGnssMode' sent with value:" << mode;
}

#include "CoreClient.h"
#include <QDBusConnection> // New: For QDBusConnection
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

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

    bool connected_new = QObject::connect(m_iface, SIGNAL(NextLandmarksUpdated(QString,int,QString,int,QString,int)),
                                          this, SIGNAL(landmarksUpdated(QString,int,QString,int,QString,int)));
    qDebug() << "CoreClient: Connection to NextLandmarksUpdated signal:" << (connected_new ? "successful" : "failed");
    // Use direct QDBusConnection for robust signal handling
    bool connected_new = QDBusConnection::systemBus().connect(
        "com.fogpass.Core",          // Service
        "/com/fogpass/Core",         // Path
        "com.fogpass.Core",          // Interface
        "NextLandmarksUpdated",      // Signal name
        this,                        // Receiver
        SLOT(onDbusLandmarksUpdated(QString,int,QString,int,QString,int)) // Slot
    );
    qDebug() << "CoreClient: Direct connection to NextLandmarksUpdated:" << (connected_new ? "successful" : "failed");

    // Connect SpeedUpdated signal from D-Bus to local signal
    bool speedConnected = QObject::connect(m_iface, SIGNAL(SpeedUpdated(int)), this, SIGNAL(speedUpdated(int)));
    qDebug() << "CoreClient: Connection to SpeedUpdated signal:" << (speedConnected ? "successful" : "failed");
}

void CoreClient::setWeatherMode(bool foggy) // Renamed and changed to bool
{
    // Fire-and-forget for now - no response handling needed
    m_iface->call(QDBus::NoBlock, "SetWeatherMode", foggy);
    qDebug() << "CoreClient: D-Bus call 'SetWeatherMode' sent with value:" << foggy;
    // No emit modeChanged(mode); here as per the "No response handling needed" instruction.
    // The UI will update when the system backend eventually sends a state update signal.
}

void CoreClient::setGnssMode(int mode)
{
    if (!m_iface->isValid()) {
        qWarning() << "CoreClient: D-Bus interface is not valid. Cannot call SetGnssMode.";
        qWarning() << "CoreClient: Last error:" << m_iface->lastError().message();
        return;
    }

    m_iface->call(QDBus::NoBlock, "SetGnssMode", mode);
    qDebug() << "CoreClient: D-Bus call 'SetGnssMode' sent with value:" << mode;
}

void CoreClient::setOperationMode(int mode)
{
    // 0=Idle, 1=Manual, 2=Auto
    m_iface->call(QDBus::NoBlock, "SetOperationMode", mode);
}

QStringList CoreClient::getAvailableRoutes()
{
    QDBusReply<QStringList> reply = m_iface->call("GetAvailableRoutes");
    if (reply.isValid()) return reply.value();
    return QStringList();
}

void CoreClient::selectRoute(const QString &routeName)
{
    m_iface->call(QDBus::NoBlock, "SelectRoute", routeName);
}

void CoreClient::clearRoute()
{
    m_iface->call(QDBus::NoBlock, "ClearRoute");
}

void CoreClient::onDbusLandmarksUpdated(const QString &l1, int d1, const QString &l2, int d2, const QString &l3, int d3)
{
    // Log the raw data received from D-Bus
    qDebug() << "CoreClient: Received landmarks ->" << l1 << d1 << "|" << l2 << d2 << "|" << l3 << d3;
    emit landmarksUpdated(l1, d1, l2, d2, l3, d3);
}

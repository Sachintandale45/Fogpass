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

void CoreClient::requestLandmarkLocations()
{
    qDebug() << "CoreClient: Requesting landmark locations from service...";
    // Make an asynchronous call to the GetLandmarkLocations method
    QDBusPendingReply<QStringList> reply = m_iface->asyncCall("GetLandmarkLocations");

    // Use a watcher to handle the reply when it arrives
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *self) {
        if (self->isValid() && self->isFinished()) {
            // Cast the watcher back to the specific reply type to access the typed result
            QDBusPendingReply<QStringList> reply = *self;
            // When the reply is received, emit our local C++ signal with the data
            emit landmarkLocationsChanged(reply.value());
        }
        // Clean up the watcher
        self->deleteLater();
    });
}

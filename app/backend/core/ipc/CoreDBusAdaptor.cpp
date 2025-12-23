#include "CoreDbusAdaptor.h"
#include "CoreState.h"

#include <QDebug>

CoreDbusAdaptor::CoreDbusAdaptor(CoreState *coreState, QObject *parent)
    : QObject(parent),
      m_coreState(coreState)
{
    // Register D-Bus service
    QDBusConnection bus = QDBusConnection::systemBus();

    if (!bus.registerService("com.fogpass.Core")) {
        qCritical() << "Failed to register D-Bus service:"
                    << bus.lastError().message();
    }

    // Register object on bus
    if (!bus.registerObject("/com/fogpass/Core",
                            this,
                            QDBusConnection::ExportAllSignals)) {
        qCritical() << "Failed to register D-Bus object:"
                    << bus.lastError().message();
    }

    // ---- Internal wiring (NOT D-Bus) ----

    // Alerts
    connect(m_coreState, &CoreState::alertRaised,
            this, &CoreDbusAdaptor::onAlertRaised,
            Qt::QueuedConnection);

    // Landmarks
    connect(m_coreState, &CoreState::nextLandmarksUpdated,
            this, &CoreDbusAdaptor::onNextLandmarksUpdated,
            Qt::QueuedConnection);

    qInfo() << "CoreDbusAdaptor initialized and exported on D-Bus";
}

// ---------- Internal → D-Bus bridge ----------

void CoreDbusAdaptor::onAlertRaised(const QString &alertId)
{
    // Emit D-Bus signal
    emit AlertRaised(alertId);
}

void CoreDbusAdaptor::onNextLandmarksUpdated(const QString &name1, int dist1,
                                             const QString &name2, int dist2,
                                             const QString &name3, int dist3)
{
    // Emit D-Bus signal
    emit NextLandmarksUpdated(name1, dist1,
                              name2, dist2,
                              name3, dist3);
}

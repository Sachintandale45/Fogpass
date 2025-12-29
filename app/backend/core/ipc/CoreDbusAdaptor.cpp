#include "CoreDbusAdaptor.h"
#include "CoreState.h"
#include "LandmarkEngine.h"

#include <QDebug>

CoreDbusAdaptor::CoreDbusAdaptor(CoreState *coreState,
                                 LandmarkEngine *landmarkEngine,
                                 QObject *parent)
    : QObject(parent),
      m_coreState(coreState),
      m_landmarkEngine(landmarkEngine)
{
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

// ---------- D-Bus → Internal bridge ----------

void CoreDbusAdaptor::SetGnssMode(int mode)
{
    // Received call from UI, forward to internal logic
    qInfo() << "[CoreDbusAdaptor] SetGnssMode request received:" << mode;
    emit gnssModeChangeRequested(mode);
}

void CoreDbusAdaptor::SetWeatherMode(bool enabled)
{
    qInfo() << "[CoreDbusAdaptor] SetWeatherMode request received:" << enabled;
    emit weatherModeChangeRequested(enabled);
}

// ---------- Navigation API ----------

void CoreDbusAdaptor::SetOperationMode(int mode)
{
    // 0=Idle, 1=Manual, 2=Auto
    m_landmarkEngine->setOperationMode(static_cast<LandmarkEngine::OperationMode>(mode));
}

QStringList CoreDbusAdaptor::GetAvailableRoutes()
{
    return m_landmarkEngine->getAvailableRoutes();
}

void CoreDbusAdaptor::SelectRoute(const QString &routeName)
{
    m_landmarkEngine->selectRoute(routeName);
}

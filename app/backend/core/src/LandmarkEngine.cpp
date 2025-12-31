#include "LandmarkEngine.h"
#include "locator.h"
#include "CoreState.h"

#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QDebug>
#include <QRegularExpression>
#include <QDir>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
LandmarkEngine::LandmarkEngine(Locator *locator,
                               CoreState *coreState,
                               QObject *parent)
    : QObject(parent),
      m_locator(locator),
      m_coreState(coreState)
{
    Q_ASSERT(m_locator);
    Q_ASSERT(m_coreState);

    connect(&m_timer, &QTimer::timeout,
            this, &LandmarkEngine::process);

    // Initialize m_next to prevent sending garbage values to UI
    for (int i = 0; i < 3; ++i) {
        m_next[i].distanceMeters = -1;
        m_next[i].name.clear();
    }
}

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------
void LandmarkEngine::setOperationMode(OperationMode mode)
{
    if (m_operationMode == mode)
        return;

    m_operationMode = mode;
    qInfo() << "[LandmarkEngine] Operation mode set to:" << mode;
    emit operationModeChanged(mode);

    // Reset state on mode change
    clearRoute();
}

QString LandmarkEngine::getSelectedRouteName() const
{
    return m_selectedRouteName;
}

QStringList LandmarkEngine::getAvailableRoutes() const
{
    QDir dir("/data/routes");
    if (!dir.exists()) {
        qWarning() << "[LandmarkEngine] Route directory does not exist:" << dir.absolutePath();
        return QStringList();
    }

    QStringList filters;
    filters << "*.csv" << "*.CSV"; // Case insensitive check
    QStringList result = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    qInfo() << "[LandmarkEngine] Scanned" << dir.absolutePath() << "- Found:" << result.size() << "files.";
    
    return result;
}

bool LandmarkEngine::selectRoute(const QString &routeName)
{  
    qDebug() << "[LandmarkEngine] selectRoute called with route:" << routeName;
    if (m_operationMode == ModeIdle) {
        qWarning() << "[LandmarkEngine] Cannot select route in IDLE mode";
        return false;
    }

    QString fullPath = "/data/routes/" + routeName;
    m_selectedRouteName = routeName;
    if (loadRouteFile(fullPath)) {
        qDebug() << "[LandmarkEngine] Route selected:" << routeName;
        m_routeSelected = true;
        start(); // Auto-start processing when route is ready
        emit routeSelected(routeName); // Announce the selection
        return true;
    }

    return false;
}

bool LandmarkEngine::loadRouteFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[LandmarkEngine] Failed to open route file:" << filePath;
        return false;
    }

    m_route.clear();
    QTextStream in(&file);

    int landmarkIndex = 1;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        Landmark lm;
        if (parseCsvLine(line, landmarkIndex, lm)) {
            m_route.push_back(lm);
            landmarkIndex++;
        }
    }

    qInfo() << "[LandmarkEngine] Loaded route with"
            << m_route.size() << "landmarks";

    m_lastClosestIndex = -1;
    return !m_route.isEmpty();
}

void LandmarkEngine::clearRoute()
{
    qDebug() << "[LandmarkEngine] clearRoute() called. Stopping engine and clearing data.";
    stop();
    m_route.clear();
    m_routeSelected = false;
    m_selectedRouteName.clear();
    m_lastClosestIndex = -1;

    // Clear the next landmarks structure so UI doesn't show stale data
    for (int i = 0; i < 3; ++i) {
        m_next[i].name.clear();
        m_next[i].distanceMeters = -1;
    }
    m_coreState->updateNextLandmarks("", -1, "", -1, "", -1);

    // If we are in Real GNSS mode, clear the locator's position to remove
    // any stale data left over from a previous simulation run.
    if (!m_isSimulation) {
        m_locator->reset();
    }
}

void LandmarkEngine::start()
{
    if (!m_timer.isActive()) {
        m_timer.start(1000); // 1 Hz
        qInfo() << "[LandmarkEngine] Started";
    }
}

void LandmarkEngine::stop()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        qInfo() << "[LandmarkEngine] Stopped";
    }
}

void LandmarkEngine::setGnssMode(int mode)
{
    m_isSimulation = (mode == 1);
    qDebug() << "[LandmarkEngine] GNSS mode updated:" << (m_isSimulation ? "SIMULATION" : "REAL");
}

// ------------------------------------------------------------
// Periodic processing
// ------------------------------------------------------------
void LandmarkEngine::process()
{
    // GATEKEEPER: Do nothing if no route is selected
    if (!m_routeSelected)
        return;

    if (!m_locator->isGnssStable()) {
        // If we have stale data (distance != -1), clear it now to prevent
        // showing frozen simulation data when switching to Real GNSS.
        if (m_next[0].distanceMeters != -1) {
            qDebug() << "[LandmarkEngine] GNSS unstable, clearing landmarks";
            for (int i = 0; i < 3; ++i) {
                m_next[i].name.clear();
                m_next[i].distanceMeters = -1;
            }
            m_coreState->updateNextLandmarks("", -1, "", -1, "", -1);
        }
        return;
    }

    Locator::Position pos = m_locator->position();

    if (m_route.isEmpty())
        return;

    computeNextLandmarks(pos.latitude, pos.longitude);

    // Only check for alerts if we are close to the next landmark
    if (m_next[0].distanceMeters > 0 && m_next[0].distanceMeters <= PREWARN_DISTANCE_METERS) {
        triggerAlerts();
    }


    // Push to CoreState → DBus → UI
    m_coreState->updateNextLandmarks(
        m_next[0].name, m_next[0].distanceMeters,
        m_next[1].name, m_next[1].distanceMeters,
        m_next[2].name, m_next[2].distanceMeters
    );
}

// ------------------------------------------------------------
// Core landmark logic
// ------------------------------------------------------------
void LandmarkEngine::computeNextLandmarks(double curLat, double curLon)
{
    int closestIdx = findClosestLandmarkIndex(curLat, curLon);
    if (closestIdx < 0)
        return;

    // If the newly found closest landmark is behind the last one, it means we are
    // still between the last landmark and the next one. To ensure we always
    // progress forward, we should stick with the last known index until we are
    // physically closer to the next one in the list.
    // The check for a route loop is to handle the case where the simulation restarts.
    if (m_lastClosestIndex != -1 && closestIdx < m_lastClosestIndex && 
        (m_lastClosestIndex - closestIdx < m_route.size() / 2)) {
        closestIdx = m_lastClosestIndex;
    }

    // "Pass" logic: If we are very close to the current target landmark (e.g. < 10m),
    // assume we have reached it and switch focus to the next one immediately.
    // This prevents the distance bouncing (2m -> 0m -> 2m) for the same landmark.
    if (closestIdx == m_lastClosestIndex) {
        const Landmark &lm = m_route[closestIdx];
        double d = distanceMeters(curLat, curLon, lm.latitude, lm.longitude);
        if (d < 10.0 && (closestIdx + 1 < m_route.size())) {
            closestIdx++;
        }
    }

    m_lastClosestIndex = closestIdx;

    for (int i = 0; i < 3; ++i) {
        int idx = closestIdx + i;
        if (idx < m_route.size()) {
            const Landmark &lm = m_route[idx];
            int dist = qRound(
                distanceMeters(curLat, curLon,
                                lm.latitude, lm.longitude));

            m_next[i].name = lm.name;
            m_next[i].distanceMeters = dist;
        } else {
            m_next[i].name.clear();
            m_next[i].distanceMeters = -1;
        }
    }
}

void LandmarkEngine::triggerAlerts()
{
    // Check distance to the next immediate landmark
    const int distanceToNext = m_next[0].distanceMeters;

    if (distanceToNext > 0 && distanceToNext <= PREWARN_DISTANCE_METERS) {
        // Raise the alert via CoreState. This will be picked up by AlertManager
        // or sent directly over D-Bus if needed.
        m_coreState->raiseAlert("LANDMARK_PREWARN");
    } else {
        m_coreState->clearAlert("LANDMARK_PREWARN");
    }
}

// ------------------------------------------------------------
// CSV parsing (FogPASS format)
// ------------------------------------------------------------
bool LandmarkEngine::parseCsvLine(const QString &line, int index, Landmark &out)
{
    // Simple format: lat,lon,speed
    const QStringList parts = line.split(',');
    if (parts.size() < 2) // We only need lat and lon
        return false;

    bool okLat, okLon;
    double lat = parts[0].toDouble(&okLat);
    double lon = parts[1].toDouble(&okLon);

    if (!okLat || !okLon)
        return false;

    out.index     = index;
    out.code      = "SIM"; // A generic code for simulated points
    out.name      = "Landmark " + QString::number(index);
    out.latitude  = lat;
    out.longitude = lon;

    return true;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
int LandmarkEngine::findClosestLandmarkIndex(double curLat, double curLon) const
{
    double minDist = 1e12;
    int bestIdx = -1;

    for (int i = 0; i < m_route.size(); ++i) {
        const Landmark &lm = m_route[i];
        double d = distanceMeters(curLat, curLon,
                                  lm.latitude, lm.longitude);
        if (d < minDist) {
            minDist = d;
            bestIdx = i;
        }
    }
    return bestIdx;
}

double LandmarkEngine::distanceMeters(double lat1, double lon1,
                                      double lat2, double lon2) const
{
    static constexpr double R = 6371000.0; // Earth radius (m)

    double dLat = qDegreesToRadians(lat2 - lat1);
    double dLon = qDegreesToRadians(lon2 - lon1);

    double a = qSin(dLat / 2) * qSin(dLat / 2) +
               qCos(qDegreesToRadians(lat1)) *
               qCos(qDegreesToRadians(lat2)) *
               qSin(dLon / 2) * qSin(dLon / 2);

    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));
    return R * c;
}

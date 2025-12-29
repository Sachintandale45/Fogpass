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
    if (m_operationMode == ModeIdle) {
        qWarning() << "[LandmarkEngine] Cannot select route in IDLE mode";
        return false;
    }

    QString fullPath = "/data/routes/" + routeName;
    m_selectedRouteName = routeName;
    if (loadRouteFile(fullPath)) {
        m_routeSelected = true;
        start(); // Auto-start processing when route is ready
        emit routeFileLoaded(fullPath); // Notify others (e.g. Simulation)
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

    bool headerSkipped = false;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // Skip header line if present
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }

        Landmark lm;
        if (parseCsvLine(line, lm)) {
            m_route.push_back(lm);
        }
    }

    qInfo() << "[LandmarkEngine] Loaded route with"
            << m_route.size() << "landmarks";

    m_lastClosestIndex = -1;
    return !m_route.isEmpty();
}

void LandmarkEngine::clearRoute()
{
    stop();
    m_route.clear();
    m_routeSelected = false;
    m_selectedRouteName.clear();
    m_lastClosestIndex = -1;
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

// ------------------------------------------------------------
// Periodic processing
// ------------------------------------------------------------
void LandmarkEngine::process()
{
    // GATEKEEPER: Do nothing if no route is selected
    if (!m_routeSelected)
        return;

    if (!m_locator->isGnssStable())
        return;

    Locator::Position pos = m_locator->position();

    if (m_route.isEmpty())
        return;

    computeNextLandmarks(pos.latitude, pos.longitude);

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

    // Enforce forward-only movement
    if (m_lastClosestIndex >= 0 &&
        closestIdx < m_lastClosestIndex) {
        closestIdx = m_lastClosestIndex;
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

// ------------------------------------------------------------
// CSV parsing (FogPASS format)
// ------------------------------------------------------------
bool LandmarkEngine::parseCsvLine(const QString &line, Landmark &out)
{
    // CSV is tab or comma separated (handle both)
    QStringList cols = line.split(QRegularExpression("[,\t]"));
    if (cols.size() < 8)
        return false;

    out.index     = cols[0].toInt();
    out.code      = cols[1].trimmed();
    out.name      = cols[2].trimmed();
    out.latitude  = cols[5].toDouble() / 100.0;
    out.longitude = cols[6].toDouble() / 100.0;

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

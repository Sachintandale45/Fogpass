#include "SimulatedGnssReader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtMath>

SimulatedGnssReader::SimulatedGnssReader(QObject *parent)
    : IGnssSource(parent)
{
    // Default: 1 Hz updates (can be made configurable later)
    qDebug() << "Simulation gnss constructor called";

    m_timer.setInterval(1000);

    connect(&m_timer, &QTimer::timeout,
            this, &SimulatedGnssReader::onTimerTick,
            Qt::QueuedConnection);
}

SimulatedGnssReader::~SimulatedGnssReader()
{
    stop();
}

void SimulatedGnssReader::onRouteSelected(const QString &routeName)
{   
    // FIX: Capture running state BEFORE loading, because loadCsvFile() calls stop()
    bool wasRunning = false;
    {
        QMutexLocker locker(&m_mutex);
        wasRunning = m_running;
    }
    
    if (routeName.isEmpty())
        qDebug() << "[SimGnss] onRouteSelected: empty route name";

    QString fullPath = "/data/routes/" + routeName;
    loadCsvFile(fullPath);

    // If we were running (active) before loading, restart the simulation now.
    QMutexLocker locker(&m_mutex);
    if (wasRunning) {
        qDebug() << "[SimGnss] onRouteSelected: restarting timer for new route";
        m_running = true;
        m_timer.start();
    }
}

// ------------------------------------------------------------
// CSV loader
// ------------------------------------------------------------
bool SimulatedGnssReader::loadCsvFile(const QString &filePath)
{
    stop(); // Stop any current simulation

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[SimGnss] Failed to open file:" << filePath;
        return false;
    }

    QVector<GnssPoint> waypoints;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const QStringList parts = line.split(',');
        if (parts.size() < 2) // lat,lon are minimum
            continue;

        bool okLat, okLon;
        GnssPoint p;
        p.latitude  = parts[0].toDouble(&okLat);
        p.longitude = parts[1].toDouble(&okLon);
        p.speedKmh  = (parts.size() > 2) ? parts[2].toDouble() : 60.0; // Default speed

        if (okLat && okLon) {
            waypoints.append(p);
        }
    }

    if (waypoints.isEmpty()) {
        qWarning() << "[SimGnss] No valid points in CSV";
        return false;
    }

    generateTrackPoints(waypoints);

    QMutexLocker locker(&m_mutex);
    m_currentIndex = 0;

    // Don't auto-start here. Let the GnssManager or onRouteSelected control it.
    // This prevents the timer from starting before the mode is officially switched.

    return true;
}

// ------------------------------------------------------------
// IGnssSource API
// ------------------------------------------------------------
bool SimulatedGnssReader::start()
{
    qDebug() << "[SimGnss] Start requested.";

    QMutexLocker locker(&m_mutex);

    // FIX: Always mark as running/active so we know to start when a route is loaded.
    m_running = true;
    m_currentIndex = 0;

    if (m_points.isEmpty()) {
        qWarning() << "[SimGnss] Start requested but no data. Waiting for route...";
        // Timer is started but onTimerTick will return early until points are loaded.
        m_timer.start(); 
        return true;
    }

    qDebug() << "[SimGnss] Timer Start from start()";
    m_timer.start();

    qDebug() << "[SimGnss] Simulation started";
    return true;
}

void SimulatedGnssReader::stop()
{
    qDebug() << "[SimGnss] Stop requested.";

    QMutexLocker locker(&m_mutex);

    if (!m_running)
        return;

    m_running = false;
    qDebug() << "[SimGnss] Timer Stop from stop()";
    m_timer.stop();
}

double SimulatedGnssReader::latitude() const
{
    QMutexLocker locker(&m_mutex);
    return m_latitude;
}

double SimulatedGnssReader::longitude() const
{
    QMutexLocker locker(&m_mutex);
    return m_longitude;
}

double SimulatedGnssReader::speedKmh() const
{
    QMutexLocker locker(&m_mutex);
    return m_speedKmh;
}

bool SimulatedGnssReader::isGnssStable() const
{
    // In simulation, the signal is always considered stable.
    return true;
}

static double simulateSpeed(double lat1, double lon1, double lat2, double lon2)
{
    double R = 6371000.0; // Earth radius (m)
    double dLat = qDegreesToRadians(lat2 - lat1);
    double dLon = qDegreesToRadians(lon2 - lon1);
    double a = qSin(dLat / 2) * qSin(dLat / 2) +
               qCos(qDegreesToRadians(lat1)) *
               qCos(qDegreesToRadians(lat2)) *
               qSin(dLon / 2) * qSin(dLon / 2);
    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));

    // Calculate distance in meters, then convert to km/h (assuming 1s interval)
    return (R * c) * 3.6;
}


// ------------------------------------------------------------
// Timer tick → advance simulation
// ------------------------------------------------------------
void SimulatedGnssReader::onTimerTick()
{
    double lat, lon, speed;
    bool should_emit = false;

    { // Scoped lock to ensure mutex is released before emitting signal
        QMutexLocker locker(&m_mutex);

        if (!m_running || m_points.isEmpty())
            return;

        const GnssPoint &p = m_points[m_currentIndex];

        // Update internal state
        m_latitude  = p.latitude;
        m_longitude = p.longitude;

        m_speedKmh = 0.0;
        if (m_currentIndex > 0) {
            const GnssPoint &prev = m_points[m_currentIndex - 1];
            m_speedKmh = simulateSpeed(prev.latitude, prev.longitude, p.latitude, p.longitude);
        }

        // Copy to local variables to emit outside the lock
        lat = m_latitude;
        lon = m_longitude;
        speed = m_speedKmh;
        should_emit = true;

        qDebug() << "[SimGnss] Tick -> Lat:" << lat << "Lon:" << lon << " | Speed:" << speed << "km/h";

        m_currentIndex = (m_currentIndex + 1) % m_points.size();
    } // Mutex is unlocked here

    if (should_emit) {
        // Emit signal outside of the locked scope to prevent deadlock
        emit positionUpdated(lat, lon, speed);
    }
}


void SimulatedGnssReader::generateTrackPoints(const QVector<GnssPoint>& waypoints)
{
    QMutexLocker locker(&m_mutex);
    m_points.clear();

    if (waypoints.size() < 2) {
        m_points = waypoints; // If only 0 or 1 point, just copy it.
        return;
    }

    for (int i = 0; i < waypoints.size() - 1; ++i) {
        const GnssPoint& startPoint = waypoints[i];
        const GnssPoint& endPoint = waypoints[i+1];

        double latStep = (endPoint.latitude - startPoint.latitude) / INTERPOLATION_STEPS;
        double lonStep = (endPoint.longitude - startPoint.longitude) / INTERPOLATION_STEPS;
        double speedStep = (endPoint.speedKmh - startPoint.speedKmh) / INTERPOLATION_STEPS;

        for (int j = 0; j < INTERPOLATION_STEPS; ++j) {
            GnssPoint p;
            p.latitude = startPoint.latitude + j * latStep;
            p.longitude = startPoint.longitude + j * lonStep;
            p.speedKmh = startPoint.speedKmh + j * speedStep;
            m_points.append(p);
        }
    }

    // Add the very last point to complete the route
    m_points.append(waypoints.last());

    qDebug() << "[SimGnss] Generated" << m_points.size() << "interpolated GNSS points from" << waypoints.size() << "waypoints.";
}

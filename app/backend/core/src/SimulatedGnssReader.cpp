#include "SimulatedGnssReader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

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

    if (routeName.isEmpty())
        qDebug() << "[SimGnss] onRouteSelected: empty route name";

    QString fullPath = "/data/routes/" + routeName;
    loadCsvFile(fullPath);

    // If we are the active GNSS source, start the simulation now that data is loaded.
    QMutexLocker locker(&m_mutex);
    if (m_running) {
        qDebug() << "[SimGnss] onRouteSelected: restarting timer for new route";
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

    if (m_points.isEmpty()) {
        qWarning() << "[SimGnss] Cannot start: no simulation data loaded";
        return false;
    }

    // if (m_running)
    //     return true;

    m_running = true;
    m_currentIndex = 0;
    m_timer.start();

    // This signal is now emitted from stop() and start() to correctly reflect state.
    emit gnssStabilityChanged(true);

    qDebug() << "[SimGnss] Simulation started";
    return true;
}

void SimulatedGnssReader::stop()
{
    QMutexLocker locker(&m_mutex);

    if (!m_running)
        return;

    m_running = true;
    m_timer.stop();

    emit gnssStabilityChanged(false);
    qDebug() << "[SimGnss] Simulation stopped";
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
    QMutexLocker locker(&m_mutex);
    return m_running;
}


// ------------------------------------------------------------
// Timer tick → advance simulation
// ------------------------------------------------------------
void SimulatedGnssReader::onTimerTick()
{
    qDebug() << "[SimGnss] Timer tick received.";

    QMutexLocker locker(&m_mutex);

    if (!m_running || m_points.isEmpty())
        return;

    const GnssPoint &p = m_points[m_currentIndex];

    m_latitude  = p.latitude;
    m_longitude = p.longitude;
    m_speedKmh  = p.speedKmh;

    qDebug() << "[SimGnss] Tick -> Lat:" << m_latitude << "Lon:" << m_longitude;

    emit positionUpdated(m_latitude, m_longitude, m_speedKmh);

    m_currentIndex++;

    // Stop at end of file (or loop — your choice later)
    if (m_currentIndex >= m_points.size()) {
        m_currentIndex = 0;
        qDebug() << "[SimGnss] Loop: restarting route";
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

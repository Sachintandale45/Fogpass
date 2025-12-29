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
        return;

    QString fullPath = "/data/routes/" + routeName;
    loadCsvFile(fullPath);
}

// ------------------------------------------------------------
// CSV loader
// ------------------------------------------------------------
bool SimulatedGnssReader::loadCsvFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[SimGnss] Failed to open file:" << filePath << "- Generating DEFAULT simulation path.";

        // Generate a simple diagonal path for testing if file is missing
        QVector<GnssPoint> points;
        double lat = 18.5204;
        double lon = 73.8567;
        for(int i=0; i<100; i++) {
            GnssPoint p;
            p.latitude = lat + (i * 0.0001);
            p.longitude = lon + (i * 0.0001);
            p.speedKmh = 40.0 + (i % 10);
            points.append(p);
        }

        QMutexLocker locker(&m_mutex);
        m_points = points;
        m_currentIndex = 0;
        return true;
    }

    QVector<GnssPoint> points;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const QStringList parts = line.split(',');
        if (parts.size() < 3)
            continue;

        bool ok1, ok2, ok3;
        GnssPoint p;
        p.latitude  = parts[0].toDouble(&ok1);
        p.longitude = parts[1].toDouble(&ok2);
        p.speedKmh  = parts[2].toDouble(&ok3);

        if (ok1 && ok2 && ok3) {
            points.append(p);
        }
    }

    if (points.isEmpty()) {
        qWarning() << "[SimGnss] No valid points in CSV";
        return false;
    }

    QMutexLocker locker(&m_mutex);
    m_points = points;
    m_currentIndex = 0;

    qDebug() << "[SimGnss] Loaded" << m_points.size() << "GNSS points";
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

    if (m_running)
        return true;

    m_running = true;
    m_currentIndex = 0;
    m_timer.start();

    qDebug() << "[SimGnss] Simulation started";
    return true;
}

void SimulatedGnssReader::stop()
{
    QMutexLocker locker(&m_mutex);

    if (!m_running)
        return;

    m_running = false;
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
    return true;        //hardcoded
}


// ------------------------------------------------------------
// Timer tick → advance simulation
// ------------------------------------------------------------
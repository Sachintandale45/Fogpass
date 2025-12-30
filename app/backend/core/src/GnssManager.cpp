#include "GnssManager.h"

#include <QDebug>

GnssManager::GnssManager(IGnssSource *realGnss,
                         IGnssSource *simGnss,
                         QObject *parent)
    : IGnssSource(parent),
      m_realGnss(realGnss),
      m_simGnss(simGnss)
{
    Q_ASSERT(m_realGnss);
    Q_ASSERT(m_simGnss);

    // Forward signals from BOTH sources
    connect(m_realGnss, &IGnssSource::gnssStabilityChanged,
            this, &GnssManager::onSourceStabilityChanged,
            Qt::QueuedConnection);

    connect(m_realGnss, &IGnssSource::positionUpdated,
            this, &GnssManager::onSourcePositionUpdated,
            Qt::QueuedConnection);

    connect(m_simGnss, &IGnssSource::gnssStabilityChanged,
            this, &GnssManager::onSourceStabilityChanged,
            Qt::QueuedConnection);

    connect(m_simGnss, &IGnssSource::positionUpdated,
            this, &GnssManager::onSourcePositionUpdated,
            Qt::QueuedConnection);

    qDebug() << "[GnssManager] Initialized (default:" << (m_mode == Mode::Simulation ? "SIMULATION" : "REAL") << ")";
}

GnssManager::~GnssManager()
{
    stop();
}

// ------------------------------------------------------------
// Lifecycle
// ------------------------------------------------------------
bool GnssManager::start()
{
    QMutexLocker locker(&m_mutex);

    if (m_mode == Mode::Real) {
        qDebug() << "*****************************************";
        qDebug() << "[GnssManager] ==> STARTING REAL GNSS <==";
        qDebug() << "*****************************************";
        m_simGnss->stop();
        return m_realGnss->start();
    } else {
        qDebug() << "*************************************************";
        qDebug() << "[GnssManager] ==> STARTING SIMULATION GNSS <==";
        qDebug() << "*************************************************";
        m_realGnss->stop();
        return m_simGnss->start();
    }
}

void GnssManager::stop()
{
    QMutexLocker locker(&m_mutex);
    m_realGnss->stop();
    m_simGnss->stop();
}

// ------------------------------------------------------------
// Data access (delegation)
// ------------------------------------------------------------
double GnssManager::latitude() const
{
    QMutexLocker locker(&m_mutex);
    return activeSource()->latitude();
}

double GnssManager::longitude() const
{
    QMutexLocker locker(&m_mutex);
    return activeSource()->longitude();
}

double GnssManager::speedKmh() const
{
    QMutexLocker locker(&m_mutex);
    return activeSource()->speedKmh();
}

bool GnssManager::isGnssStable() const
{
    QMutexLocker locker(&m_mutex);
    return activeSource()->isGnssStable();
}

// ------------------------------------------------------------
// Mode switching (runtime)
// ------------------------------------------------------------
void GnssManager::setMode(GnssManager::Mode mode)
{
    QMutexLocker locker(&m_mutex);

    qDebug() << "[GnssManager] Received request to set mode to:" << (mode == Mode::Simulation ? "SIMULATION" : "REAL");

    if (m_mode == mode) {
        qDebug() << "[GnssManager] Already in requested mode. No change.";
        return; // no-op
    }

    qDebug() << "[GnssManager] Switching GNSS mode from" << (m_mode == Mode::Simulation ? "SIM" : "REAL") << "to" << (mode == Mode::Simulation ? "SIM" : "REAL");

    // Stop current source
    activeSource()->stop(); // This stops the *old* source

    m_mode = mode;

    // Start new source
    activeSource()->start();

    emit gnssModeChanged(m_mode);
}

// ------------------------------------------------------------
// Signal forwarding (only from ACTIVE source)
// ------------------------------------------------------------
void GnssManager::onSourceStabilityChanged(bool stable)
{
    bool shouldEmit = false;
    {
        QMutexLocker locker(&m_mutex);

        // Emit only if this signal comes from active source
        if (sender() == activeSource()) {
            shouldEmit = true;
        }
    }
    if (shouldEmit)
        emit gnssStabilityChanged(stable);
}

void GnssManager::onSourcePositionUpdated(double lat,
                                          double lon,
                                          double speed)
{
    bool shouldEmit = false;
    {
        QMutexLocker locker(&m_mutex);
        if (sender() == activeSource()) {
            shouldEmit = true;
        }
    }
    if (shouldEmit)
        emit positionUpdated(lat, lon, speed);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
IGnssSource *GnssManager::activeSource() const
{
    return (m_mode == Mode::Real) ? m_realGnss : m_simGnss;
}

#include "Locator.h"
#include "GnssReader.h"

#include <QDebug>

Locator::Locator(GnssReader* gnssReader, QObject* parent)
    : QObject(parent),
      m_gnss(gnssReader)
{
    Q_ASSERT(m_gnss != nullptr);
}

void Locator::update()
{
    if (m_gnss->isGnssStable()) {
        updateFromGnss();
    } else {
        updateFromImu();
    }
}

void Locator::updateFromGnss()
{
    Position pos;
    pos.latitude  = m_gnss->latitude();
    pos.longitude = m_gnss->longitude();
    pos.speedKmh  = m_gnss->speedKmh();

    {
        QMutexLocker lock(&m_mutex);
        m_currentPosition = pos;
        m_gnssStable = true;
    }

    qDebug() << "[Locator] GNSS position:"
             << pos.latitude << pos.longitude
             << "Speed:" << pos.speedKmh << "km/h";
}

void Locator::updateFromImu()
{
    // Placeholder for future dead-reckoning / sensor fusion
    // For now, keep last known position

    {
        QMutexLocker lock(&m_mutex);
        m_gnssStable = false;
    }

    qDebug() << "[Locator] GNSS unstable, using IMU-based estimation (TODO)";
}

Locator::Position Locator::position() const
{
    QMutexLocker lock(&m_mutex);
    return m_currentPosition;
}

bool Locator::isGnssStable() const
{
    QMutexLocker lock(&m_mutex);
    return m_gnssStable;
}

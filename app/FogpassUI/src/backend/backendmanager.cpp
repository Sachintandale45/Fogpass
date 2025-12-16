#include "backendmanager.h"
#include <QDebug>

BackendManager::BackendManager(QObject *parent)
    : QObject(parent)
    , m_batteryLevel(95)
{
    m_timer.setInterval(500); // two updates per second
    m_timer.setTimerType(Qt::CoarseTimer);
    connect(&m_timer, &QTimer::timeout, this, &BackendManager::onTimeout);
}

int BackendManager::batteryLevel() const
{
    return m_batteryLevel;
}

void BackendManager::start()
{
    if (!m_timer.isActive())
        m_timer.start();
}

void BackendManager::onTimeout()
{
    if (m_batteryLevel > 25) {
        m_batteryLevel -= 1; // simulate gradual drop
        emit batteryLevelChanged();
    } else {
        m_timer.stop();
    }
}

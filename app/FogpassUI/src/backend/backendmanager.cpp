#include "backendmanager.h"
#include <QDebug>

BackendManager::BackendManager(QObject *parent)
    : QObject(parent)
    , m_batteryLevel(87) // Start from the same default as AppSettings
{
    m_timer.setInterval(1000); // Update once per second
    m_timer.setTimerType(Qt::CoarseTimer);
    connect(&m_timer, &QTimer::timeout, this, &BackendManager::onTimeout);
}

void BackendManager::start()
{
    qDebug() << "BackendManager: Starting battery simulation.";
    if (!m_timer.isActive())
        m_timer.start();
}

void BackendManager::onTimeout()
{
    if (m_batteryLevel > 25) { // Stop at 25
        m_batteryLevel -= 1; // simulate gradual drop
        emit batteryLevelUpdated(m_batteryLevel); // Emit signal with the new value
    } else {
        m_timer.stop();
    }
}
